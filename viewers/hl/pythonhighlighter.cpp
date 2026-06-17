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

#include "pythonhighlighter.h"

PythonHighlighter::PythonHighlighter(const Theme& theme, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , rules(QList<HighlighterRule>())
    , stringFormat(theme.stringLiteral())
{
    const QTextCharFormat keywordFormat = theme.keyword();
    const std::array<std::string, 38> keywords = {
        "and",
        "as",
        "assert",
        "async",
        "await",
        "break",
        "case",
        "class",
        "continue",
        "def",
        "del",
        "elif",
        "else",
        "except",
        "False",
        "finally",
        "for",
        "from",
        "global",
        "if",
        "import",
        "in",
        "is",
        "lambda",
        "match",
        "None",
        "nonlocal",
        "not",
        "or",
        "pass",
        "raise",
        "return",
        "True",
        "try",
        "type",
        "while",
        "with",
        "yield",
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
    const std::array<std::string, 16> operators = {
        "+", "-", "*", "/", "%", "^", "&", "|",
        "~", "!", "=", "<", ">", "[", "]", "@"
    };

    rule.format = theme.operators();
    for (const std::string& op : operators)
    {
        rule.pattern = QRegularExpression(QString::fromStdString("\\" + op));
        rules.append(rule);
    }

    // numeric literals: matches hexadecimal, octal, binary and decimal notation
    rule.pattern = QRegularExpression(QStringLiteral("\\b(0[xX][0-9a-fA-F]+|0[oO][0-7]+|0[bB][01]+|\\-?[0-9]+(\\.[0-9]+)?([eE][0-9]+)?)\\b"));
    rule.format = theme.constants();
    rules.append(rule);

    // string literal, single quotes
    rule.pattern = QRegularExpression(QStringLiteral("'.*[^\\\\]'"), QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    rules.append(rule);

    // string literal, double quotes
    rule.pattern = QRegularExpression(QStringLiteral("\".*[^\\\\]\""), QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    rules.append(rule);

    // triple-quoted string
    rule.pattern = QRegularExpression(QStringLiteral("\"\"\".*[^\\\\]\"\"\""), QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    rules.append(rule);

    // single-line comments
    rule.pattern = QRegularExpression(QStringLiteral("#[^\n]*"));
    rule.format = theme.comment();
    rules.append(rule);
}

void PythonHighlighter::highlightBlock(const QString &text)
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

    setCurrentBlockState(NotInTripleQuotedStringState);

    int startIndex = 0;
    bool hasStart = false;

    if (previousBlockState() != IsInTripleQuotedStringState)
    {
        startIndex = text.indexOf(QStringLiteral("\"\"\""));
        hasStart = startIndex >= 0;
    }

    while (startIndex >= 0)
    {
        const qsizetype from = hasStart ? startIndex + 1 : startIndex;
        const qsizetype endIndex = text.indexOf(QStringLiteral("\"\"\""), from);
        qsizetype tripStringLength = 0;
        if (endIndex == -1)
        {
            setCurrentBlockState(IsInTripleQuotedStringState);
            tripStringLength = text.length() - startIndex;
        }
        else
        {
            setCurrentBlockState(NotInTripleQuotedStringState);
            tripStringLength = endIndex - startIndex + 3;
        }
        setFormat(startIndex, tripStringLength, stringFormat);
        startIndex = text.indexOf(QStringLiteral("\"\"\""), startIndex + tripStringLength);
        hasStart = startIndex >= 0;
    }
}
