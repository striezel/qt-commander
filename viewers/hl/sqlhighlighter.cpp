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

#include "sqlhighlighter.h"

SqlHighlighter::SqlHighlighter(const Theme& theme, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , rules(QList<HighlighterRule>())
    , commentFormat(theme.comment())
{
    const QTextCharFormat keywordFormat = theme.keyword();
    const std::array<std::string, 391> keywords = {
        "ABS",
        "ABSENT",
        "ACOS",
        "ALL",
        "ALLOCATE",
        "ALTER",
        "AND",
        "ANY",
        "ANY_VALUE",
        "ARE",
        "ARRAY",
        "ARRAY_AGG",
        "ARRAY_MAX_CARDINALITY",
        "AS",
        "ASENSITIVE",
        "ASIN",
        "ASYMMETRIC",
        "AT",
        "ATAN",
        "ATOMIC",
        "AUTHORIZATION",
        "AVG",
        "BEGIN",
        "BEGIN_FRAME",
        "BEGIN_PARTITION",
        "BETWEEN",
        "BIGINT",
        "BINARY",
        "BLOB",
        "BOOLEAN",
        "BOTH",
        "BTRIM",
        "BY",
        "CALL",
        "CALLED",
        "CARDINALITY",
        "CASCADED",
        "CASE",
        "CAST",
        "CEIL",
        "CEILING",
        "CHAR",
        "CHARACTER",
        "CHARACTER_LENGTH",
        "CHAR_LENGTH",
        "CHECK",
        "CLASSIFIER",
        "CLOB",
        "CLOSE",
        "COALESCE",
        "COLLATE",
        "COLLECT",
        "COLUMN",
        "COMMENT",
        "COMMIT",
        "CONDITION",
        "CONNECT",
        "CONSTRAINT",
        "CONTAINS",
        "CONVERT",
        "COPY",
        "CORR",
        "CORRESPONDING",
        "COS",
        "COSH",
        "COUNT",
        "COVAR_POP",
        "COVAR_SAMP",
        "CREATE",
        "CROSS",
        "CUBE",
        "CUME_DIST",
        "CURRENT",
        "CURRENT_CATALOG",
        "CURRENT_DATE",
        "CURRENT_DEFAULT_TRANSFORM_GROUP",
        "CURRENT_PATH",
        "CURRENT_ROLE",
        "CURRENT_ROW",
        "CURRENT_SCHEMA",
        "CURRENT_TIME",
        "CURRENT_TIMESTAMP",
        "CURRENT_TRANSFORM_GROUP_FOR_TYPE",
        "CURRENT_USER",
        "CURSOR",
        "CYCLE",
        "DATE",
        "DAY",
        "DEALLOCATE",
        "DEC",
        "DECFLOAT",
        "DECIMAL",
        "DECLARE",
        "DEFAULT",
        "DEFINE",
        "DELETE",
        "DENSE_RANK",
        "DEREF",
        "DESCRIBE",
        "DETERMINISTIC",
        "DISCONNECT",
        "DISTINCT",
        "DO",
        "DOUBLE",
        "DROP",
        "DYNAMIC",
        "EACH",
        "ELEMENT",
        "ELSE",
        "ELSEIF",
        "EMPTY",
        "END",
        "SQL-2023 ",
        "END_FRAME",
        "END_PARTITION",
        "ENGINE",
        "EQUALS",
        "ESCAPE",
        "EVERY",
        "EXCEPT",
        "EXEC",
        "EXECUTE",
        "EXISTS",
        "EXP",
        "EXTERNAL",
        "EXTRACT",
        "FALSE",
        "FETCH",
        "FILTER",
        "FIRST_VALUE",
        "FLOAT",
        "FLOOR",
        "FOR",
        "FOREIGN",
        "FRAME_ROW",
        "FREE",
        "FROM",
        "FULL",
        "FUNCTION",
        "FUSION",
        "GET",
        "GLOBAL",
        "GRANT",
        "GREATEST",
        "GROUP",
        "GROUPING",
        "GROUPS",
        "HANDLER",
        "HAVING",
        "HOLD",
        "HOUR",
        "IDENTITY",
        "IF",
        "IN",
        "INDICATOR",
        "INITIAL",
        "INNER",
        "INOUT",
        "INSENSITIVE",
        "INSERT",
        "INT",
        "INTEGER",
        "INTERSECT",
        "INTERSECTION",
        "INTERVAL",
        "INTO",
        "IS",
        "ITERATE",
        "JOIN",
        "JSON",
        "JSON_ARRAY",
        "JSON_ARRAYAGG",
        "JSON_EXISTS",
        "JSON_OBJECT",
        "JSON_OBJECTAGG",
        "JSON_QUERY",
        "JSON_SCALAR",
        "JSON_SERIALIZE",
        "JSON_TABLE",
        "JSON_TABLE_PRIMITIVE",
        "JSON_VALUE",
        "KEY",
        "LAG",
        "LANGUAGE",
        "LARGE",
        "LAST_VALUE",
        "LATERAL",
        "LEAD",
        "LEADING",
        "LEAST",
        "LEAVE",
        "LEFT",
        "LIKE",
        "LIKE_REGEX",
        "LISTAGG",
        "LN",
        "LOCAL",
        "LOCALTIME",
        "LOCALTIMESTAMP",
        "LOG",
        "LOG10",
        "LOOP",
        "LOWER",
        "LPAD",
        "LTRIM",
        "MATCH",
        "MATCHES",
        "MATCH_NUMBER",
        "MATCH_RECOGNIZE",
        "MAX",
        "MEMBER",
        "MERGE",
        "METHOD",
        "MIN",
        "MINUTE",
        "MOD",
        "MODIFIES",
        "MODULE",
        "MONTH",
        "MULTISET",
        "NATIONAL",
        "NATURAL",
        "NCHAR",
        "NCLOB",
        "NEW",
        "NO",
        "NONE",
        "NORMALIZE",
        "NOT",
        "NTH_VALUE",
        "NTILE",
        "NULL",
        "NULLIF",
        "NUMERIC",
        "OCCURRENCES_REGEX",
        "OCTET_LENGTH",
        "OF",
        "OFFSET",
        "OLD",
        "OMIT",
        "ON",
        "ONE",
        "ONLY",
        "OPEN",
        "OR",
        "ORDER",
        "OUT",
        "OUTER",
        "OVER",
        "OVERLAPS",
        "OVERLAY",
        "PARAMETER",
        "PARTITION",
        "PATTERN",
        "PER",
        "PERCENT",
        "PERCENTILE_CONT",
        "PERCENTILE_DISC",
        "PERCENT_RANK",
        "PERIOD",
        "PORTION",
        "POSITION",
        "POSITION_REGEX",
        "POWER",
        "PRECEDES",
        "PRECISION",
        "PREPARE",
        "PRIMARY",
        "PROCEDURE",
        "PTF",
        "RANGE",
        "RANK",
        "READS",
        "REAL",
        "RECURSIVE",
        "REF",
        "REFERENCES",
        "REFERENCING",
        "REGR_AVGX",
        "REGR_AVGY",
        "REGR_COUNT",
        "REGR_INTERCEPT",
        "REGR_R2",
        "REGR_SLOPE",
        "REGR_SXX",
        "REGR_SXY",
        "REGR_SYY",
        "RELEASE",
        "REPEAT",
        "RESIGNAL",
        "RESULT",
        "RETURN",
        "RETURNS",
        "REVOKE",
        "RIGHT",
        "ROLLBACK",
        "ROLLUP",
        "ROW",
        "ROWS",
        "ROW_NUMBER",
        "RPAD",
        "RTRIM",
        "RUNNING",
        "SAVEPOINT",
        "SCOPE",
        "SCROLL",
        "SEARCH",
        "SECOND",
        "SEEK",
        "SELECT",
        "SENSITIVE",
        "SESSION_USER",
        "SET",
        "SHOW",
        "SIGNAL",
        "SIMILAR",
        "SIN",
        "SINH",
        "SKIP",
        "SMALLINT",
        "SOME",
        "SPECIFIC",
        "SPECIFICTYPE",
        "SQL",
        "SQLEXCEPTION",
        "SQLSTATE",
        "SQLWARNING",
        "SQRT",
        "START",
        "STATIC",
        "STDDEV_POP",
        "STDDEV_SAMP",
        "SUBMULTISET",
        "SUBSET",
        "SUBSTRING",
        "SUBSTRING_REGEX",
        "SUCCEEDS",
        "SUM",
        "SYMMETRIC",
        "SYSTEM",
        "SYSTEM_TIME",
        "SYSTEM_USER",
        "TABLE",
        "TABLESAMPLE",
        "TAN",
        "TANH",
        "THEN",
        "TIME",
        "TIMESTAMP",
        "TIMEZONE_HOUR",
        "TIMEZONE_MINUTE",
        "TO",
        "TRAILING",
        "TRANSLATE",
        "TRANSLATE_REGEX",
        "TRANSLATION",
        "TREAT",
        "TRIGGER",
        "TRIM",
        "TRIM_ARRAY",
        "TRUE",
        "TRUNCATE",
        "UESCAPE",
        "UNION",
        "UNIQUE",
        "UNKNOWN",
        "UNNEST",
        "UNTIL",
        "UPDATE",
        "UPPER",
        "USER",
        "USING",
        "VALUE",
        "VALUES",
        "VALUE_OF",
        "VARBINARY",
        "VARCHAR",
        "VARYING",
        "VAR_POP",
        "VAR_SAMP",
        "VERSIONING",
        "WHEN",
        "WHENEVER",
        "WHERE",
        "WHILE",
        "WIDTH_BUCKET",
        "WINDOW",
        "WITH",
        "WITHIN",
        "WITHOUT",
        "YEAR",
    };

    HighlighterRule rule;

    for (const std::string& keyword : keywords)
    {
        rule.pattern = QRegularExpression(QString::fromStdString("\\b" + keyword + "\\b"), QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        rules.append(rule);
    }

    // numeric literals: decimal notation
    rule.pattern = QRegularExpression(QStringLiteral("\\b\\-?[0-9]+(\\.[0-9]+)?([eE][0-9]+)?\\b"));
    rule.format = theme.constants();
    rules.append(rule);

    // string literal
    rule.pattern = QRegularExpression(QStringLiteral("'.*[^\\\\]'"), QRegularExpression::InvertedGreedinessOption);
    rule.format = theme.stringLiteral();
    rules.append(rule);

    // single-line comments
    rule.pattern = QRegularExpression(QStringLiteral("--[^\n]*"));
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

void SqlHighlighter::highlightBlock(const QString &text)
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
