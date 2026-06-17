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

#include "rusthighlighter.h"

RustHighlighter::RustHighlighter(const Theme& theme, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , rules(QList<HighlighterRule>())
    , commentFormat(theme.comment())
{
    const QTextCharFormat keywordFormat = theme.keyword();
    const std::array<std::string, 42> keywords = {
        "as",
        "async",
        "await",
        "break",
        "const",
        "continue",
        "crate",
        "dyn",
        "else",
        "enum",
        "extern",
        "false",
        "fn",
        "for",
        "if",
        "impl",
        "in",
        "let",
        "loop",
        "macro_rules",
        "match",
        "mod",
        "move",
        "mut",
        "pub",
        "raw",
        "ref",
        "return",
        "safe",
        "self",
        "Self",
        "static",
        "struct",
        "super",
        "trait",
        "true",
        "type",
        "union",
        "unsafe",
        "use",
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
    const std::array<std::string, 14> operators = {
        "+", "-", "*", "/", "%", "^", "&", "|",
        "!", "=", "<", ">", "[", "]"
    };

    rule.format = theme.operators();
    for (const std::string& op : operators)
    {
        rule.pattern = QRegularExpression(QString::fromStdString("\\" + op));
        rules.append(rule);
    }

    // numeric literals: matches hexadecimal, octal, binary and decimal notation
    rule.pattern = QRegularExpression(QStringLiteral("\\b(0x[0-9a-fA-F]+|0o[0-7]+|0b[01]+|\\-?[0-9]+(\\.[0-9]+)?([eE][0-9]+)?)\\b"));
    rule.format = theme.constants();
    rules.append(rule);

    // macros
    rule.pattern = QRegularExpression(QStringLiteral("\\b[a-zA-Z0-9_]+!"));
    rule.format = theme.special();
    rules.append(rule);

    // attributes
    rule.pattern = QRegularExpression(QStringLiteral("#\\[[^\n]+\\]"));
    rule.format = theme.special();
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

void RustHighlighter::highlightBlock(const QString &text)
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
