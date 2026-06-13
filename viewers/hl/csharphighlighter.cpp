/*
 -------------------------------------------------------------------------------
    This file is part of the Qt Commander file manager.
    Copyright (C) 2026  Dirk Stolle

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------------
*/


#include "csharphighlighter.h"

CSharpHighlighter::CSharpHighlighter(const Theme& theme, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , rules(QList<HighlighterRule>())
    , commentFormat(theme.comment())
{
    const QTextCharFormat keywordFormat = theme.keyword();
    const std::array<std::string, 80> keywords = {
        "abstract",
        "as",
        "async",
        "base",
        "bool",
        "break",
        "byte",
        "case",
        "catch",
        "char",
        "checked",
        "class",
        "const",
        "continue",
        "decimal",
        "default",
        "delegate",
        "do",
        "double",
        "dynamic",
        "else",
        "enum",
        "event",
        "explicit",
        "extern",
        "false",
        "finally",
        "fixed",
        "float",
        "for",
        "foreach",
        "goto",
        "if",
        "implicit",
        "in",
        "int",
        "interface",
        "internal",
        "is",
        "lock",
        "long",
        "namespace",
        "new",
        "null",
        "object",
        "operator",
        "out",
        "override",
        "params",
        "private",
        "protected",
        "public",
        "readonly",
        "ref",
        "return",
        "sbyte",
        "sealed",
        "short",
        "signed",
        "sizeof",
        "stackalloc",
        "static",
        "string",
        "struct",
        "switch",
        "this",
        "throw",
        "true",
        "try",
        "typeof",
        "uint",
        "ulong",
        "unchecked",
        "unsafe",
        "using",
        "virtual",
        "void",
        "volatile",
        "where",
        "while",
    };

    HighlighterRule rule;

    constexpr unsigned int words_per_regex = 15;
    unsigned int word_count = 0;
    QString words;

    for (const std::string& keyword : keywords)
    {
        if (words.isEmpty())
        {
            words = QString::fromStdString(keyword);
        }
        else
        {
            words += QStringLiteral("|") + QString::fromStdString(keyword);
        }
        ++word_count;
        if (word_count >= words_per_regex)
        {
            rule.pattern = QRegularExpression("\\b(" + words + ")\\b");
            rule.format = keywordFormat;
            rules.append(rule);

            words.clear();
            word_count = 0;
        }
    }

    if (word_count > 0)
    {
        rule.pattern = QRegularExpression("\\b(" + words + ")\\b");
        rule.format = keywordFormat;
        rules.append(rule);
    }

    // operators
    const std::array<std::string, 15> operators = {
        "+", "-", "*", "/", "%", "^", "&", "|",
        "~", "!", "=", "<", ">", "[", "]"
    };

    rule.format = theme.operators();
    for (const std::string& op : operators)
    {
        rule.pattern = QRegularExpression(QString::fromStdString("\\" + op));
        rules.append(rule);
    }

    // numeric literals: matches hexadecimal, binary and decimal notation
    rule.pattern = QRegularExpression(QStringLiteral("\\b(0x[0-9a-fA-F]+|0b[01]+|\\-?[0-9]+(\\.[0-9]+)?([eE][0-9]+)?)\\b"));
    rule.format = theme.constants();
    rules.append(rule);

    const QTextCharFormat stringLiteralFormat = theme.stringLiteral();
    // char literal
    rule.pattern = QRegularExpression(QStringLiteral("'.'"), QRegularExpression::InvertedGreedinessOption);
    rule.format = stringLiteralFormat;
    rules.append(rule);

    // string literal
    rule.pattern = QRegularExpression(QStringLiteral("\".*[^\\\\]\""), QRegularExpression::InvertedGreedinessOption);
    rule.format = stringLiteralFormat;
    rules.append(rule);

    // single-line comments
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = commentFormat;
    rules.append(rule);

    // multi-line comments
    rule.pattern = QRegularExpression(
        QStringLiteral("/\\*.*\\*/"),
        QRegularExpression::InvertedGreedinessOption
            | QRegularExpression::DotMatchesEverythingOption);
    rule.format = commentFormat;
    rules.append(rule);
}

void CSharpHighlighter::highlightBlock(const QString &text)
{
    for (const HighlighterRule& rule : std::as_const(rules))
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            const QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(NotInMultiLineCommentState);

    int startIndex = 0;
    if (previousBlockState() != IsInMultiLineCommentState)
    {
        startIndex = text.indexOf(QStringLiteral("/*"));
    }

    while (startIndex >= 0)
    {
        const qsizetype endIndex = text.indexOf(QStringLiteral("*/"), startIndex);
        qsizetype commentLength = 0;
        if (endIndex == -1)
        {
            setCurrentBlockState(IsInMultiLineCommentState);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + 2;
        }
        setFormat(startIndex, commentLength, commentFormat);
        startIndex = text.indexOf(QStringLiteral("/*"), startIndex + commentLength);
    }
}
