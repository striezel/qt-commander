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

#include "pascalhighlighter.h"

PascalHighlighter::PascalHighlighter(const Theme& theme, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , rules(QList<HighlighterRule>())
    , commentFormat(theme.comment())
{
    const QTextCharFormat keywordFormat = theme.keyword();
    const std::array<std::string, 73> keywords = {
        "and",
        "array",
        "as",
        "asm",
        "begin",
        "break",
        "byte",
        "case",
        "class",
        "const",
        "constructor",
        "continue",
        "destructor",
        "div",
        "do",
        "downto",
        "else",
        "end",
        "except",
        "exit",
        "exports",
        "file",
        "finalization",
        "finally",
        "for",
        "function",
        "goto",
        "if",
        "implementation",
        "in",
        "inherited",
        "initialization",
        "inline",
        "interface",
        "is",
        "label",
        "library",
        "mod",
        "new",
        "nil",
        "not",
        "object",
        "of",
        "on",
        "operator",
        "or",
        "packed",
        "private",
        "procedure",
        "program",
        "property",
        "protected",
        "public",
        "published",
        "raise",
        "record",
        "repeat",
        "self",
        "set",
        "shl",
        "shr",
        "string",
        "then",
        "to",
        "try",
        "type",
        "unit",
        "until",
        "uses",
        "var",
        "while",
        "with",
        "xor",
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
            rule.pattern = QRegularExpression("\\b(" + words + ")\\b", QRegularExpression::CaseInsensitiveOption);
            rule.format = keywordFormat;
            rules.append(rule);

            words.clear();
            word_count = 0;
        }
    }
    if (word_count > 0)
    {
        rule.pattern = QRegularExpression("\\b(" + words + ")\\b", QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        rules.append(rule);
    }

    // operators
    const std::array<std::string, 8> operators = {
        "+", "-", "*", "/", ":=", "=", "<", ">"
    };

    rule.format = theme.operators();
    for (const std::string& op : operators)
    {
        rule.pattern = QRegularExpression(QString::fromStdString("\\" + op));
        rules.append(rule);
    }

    // numeric literals: decimal notation
    rule.pattern = QRegularExpression(QStringLiteral("\\b\\-?[0-9]+(\\.[0-9]+)?([eE][0-9]+)?\\b"));
    rule.format = theme.constants();
    rules.append(rule);

    // boolean literals
    rule.pattern = QRegularExpression(QStringLiteral("\\b(true|false)\\b"), QRegularExpression::CaseInsensitiveOption);
    rule.format = theme.constants();
    rules.append(rule);

    // string literal
    rule.pattern = QRegularExpression(QStringLiteral("'.*'"), QRegularExpression::InvertedGreedinessOption);
    rule.format = theme.stringLiteral();
    rules.append(rule);

    // single-line comments
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = commentFormat;
    rules.append(rule);

    // multi-line comments
    rule.pattern = QRegularExpression(
        QStringLiteral("\\{.*\\}"),
        QRegularExpression::InvertedGreedinessOption
            | QRegularExpression::DotMatchesEverythingOption);
    rule.format = commentFormat;
    rules.append(rule);
}

void PascalHighlighter::highlightBlock(const QString &text)
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
        startIndex = text.indexOf(QStringLiteral("{"));
    }

    while (startIndex >= 0)
    {
        const qsizetype endIndex = text.indexOf(QStringLiteral("}"), startIndex);
        qsizetype commentLength = 0;
        if (endIndex == -1)
        {
            setCurrentBlockState(IsInMultiLineCommentState);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + 1;
        }
        setFormat(startIndex, commentLength, commentFormat);
        startIndex = text.indexOf(QStringLiteral("{"), startIndex + commentLength);
    }
}
