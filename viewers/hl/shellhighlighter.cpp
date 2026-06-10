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

#include "shellhighlighter.h"

ShellHighlighter::ShellHighlighter(const Theme& theme, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , rules(QList<HighlighterRule>())
{
    const QTextCharFormat keywordFormat = theme.keyword();
    const std::array<std::string, 16> keywords = {
        "case",
        "coproc",
        "do",
        "done",
        "elif",
        "else",
        "esac",
        "fi",
        "for",
        "function",
        "if",
        "in",
        "select",
        "then",
        "until",
        "while",
    };

    HighlighterRule rule;

    for (const std::string& keyword : keywords)
    {
        rule.pattern = QRegularExpression(QString::fromStdString("\\b" + keyword + "\\b"));
        rule.format = keywordFormat;
        rules.append(rule);
    }

    // variables
    rule.pattern = QRegularExpression(QStringLiteral("\\$[a-zA-z0-9_\\?]+"));
    rule.format = theme.constants(); // Oh the irony ...
    rules.append(rule);

    rule.pattern = QRegularExpression(QStringLiteral("\\$\\{[a-zA-z0-9_]+\\}"));
    rules.append(rule);

    const std::array<QString, 18> operators = {
        QRegularExpression::escape("|"),
        QRegularExpression::escape("&"),
        QRegularExpression::escape(";"),
        QRegularExpression::escape("<"),
        QRegularExpression::escape(">"),
        QRegularExpression::escape("="),
        QRegularExpression::escape("=="),
        QRegularExpression::escape("!="),
        QRegularExpression::escape("=~"),
        "[ \t]\\-a[ \t]", "[ \t]\\-o[ \t]", "[ \t]\\-eq[ \t]",
        "[ \t]\\-gt[ \t]", "[ \t]\\-le[ \t]", "[ \t]\\-lt[ \t]",
        "[ \t]\\-ne[ \t]", "[ \t]\\-nt[ \t]", "[ \t]\\-ot[ \t]"
    };
    rule.format = theme.operators();
    for (const QString& op : operators)
    {
        rule.pattern = QRegularExpression(op);
        rules.append(rule);
    }

    const QTextCharFormat stringLiteralFormat = theme.stringLiteral();
    // single-quoted string literal
    rule.pattern = QRegularExpression(QStringLiteral("'.*[^\\\\]'"), QRegularExpression::InvertedGreedinessOption);
    rule.format = stringLiteralFormat;
    rules.append(rule);

    // double-quoted string literal
    rule.pattern = QRegularExpression(QStringLiteral("\".*[^\\\\]\""), QRegularExpression::InvertedGreedinessOption);
    rule.format = stringLiteralFormat;
    rules.append(rule);

    // single-line comments
    rule.pattern = QRegularExpression(QStringLiteral("#[^\n]*"));
    rule.format = theme.comment();
    rules.append(rule);
}

void ShellHighlighter::highlightBlock(const QString &text)
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
}
