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

#include "cpphighlighter.h"
#include <array>
#include <string>

CppHighlighter::CppHighlighter(const Theme& theme, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , rules(QList<HighlighterRule>())
    , commentFormat(theme.comment())
{
    const QTextCharFormat keywordFormat = theme.keyword();
    const std::array<std::string, 81> keywords = {
        "alignas",
        "alignof",
        "asm",
        "auto",
        "bool",
        "break",
        "case",
        "catch",
        "char",
        "char8_t",
        "char16_t",
        "char32_t",
        "class",
        "concept",
        "const",
        "constexpr",
        "const_cast",
        "continue",
        "co_await",
        "co_return",
        "co_yield",
        "decltype",
        "default",
        "delete",
        "do",
        "double",
        "dynamic_cast",
        "else",
        "enum",
        "explicit",
        "export",
        "extern",
        "false",
        "final",
        "float",
        "for",
        "friend",
        "goto",
        "if",
        "inline",
        "int",
        "long",
        "mutable",
        "namespace",
        "new",
        "noexcept",
        "nullptr",
        "operator",
        "override",
        "private",
        "protected",
        "public",
        "register",
        "reinterpret_cast",
        "requires",
        "return",
        "short",
        "signed",
        "sizeof",
        "static",
        "static_assert",
        "static_cast",
        "struct",
        "switch",
        "template",
        "this",
        "thread_local",
        "throw",
        "true",
        "try",
        "typedef",
        "typeid",
        "typename",
        "union",
        "unsigned",
        "using",
        "virtual",
        "void",
        "volatile",
        "wchar_t",
        "while",
    };

    HighlighterRule rule;

    for (const std::string& keyword : keywords)
    {
        rule.pattern = QRegularExpression(QString::fromStdString("\\b" + keyword + "\\b"));
        rule.format = keywordFormat;
        rules.append(rule);
    }

    const std::array<std::string, 12> preprocessor = {
        "define",
        "error",
        "if",
        "ifdef",
        "ifndef",
        "include",
        "elif",
        "else",
        "endif",
        "pragma",
        "undef",
        "warning",
    };

    const QTextCharFormat preprocessorFormat = theme.preprocessor();

    for (const std::string& directive : preprocessor)
    {
        rule.pattern = QRegularExpression(QString::fromStdString("#[ \\t]*" + directive + "\\b"));
        rule.format = preprocessorFormat;
        rules.append(rule);
    }

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

void CppHighlighter::highlightBlock(const QString &text)
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
