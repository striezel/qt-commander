#include "luahighlighter.h"

LuaHighlighter::LuaHighlighter(const Theme& theme, QTextDocument* parent)
    : QSyntaxHighlighter(parent)
    , rules(QList<HighlighterRule>())
{
    const QTextCharFormat keywordFormat = theme.keyword();
    const std::array<std::string, 23> keywords = {
        "and",
        "break",
        "do",
        "else",
        "elseif",
        "end",
        "false",
        "for",
        "function",
        "global",
        "goto",
        "if",
        "in",
        "local",
        "nil",
        "not",
        "or",
        "repeat",
        "return",
        "then",
        "true",
        "until",
        "while",
    };

    HighlighterRule rule;

    constexpr unsigned int words_per_regex = 12;
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
    const std::array<std::string, 13> operators = {
        "+", "-", "*", "/", "%", "#", "&", "~", "|", ":", "=", "<", ">"
    };

    rule.format = theme.operators();
    for (const std::string& op : operators)
    {
        rule.pattern = QRegularExpression(QString::fromStdString("\\" + op));
        rules.append(rule);
    }

    // numeric literals: matches hexadecimal and decimal notation
    rule.pattern = QRegularExpression(QStringLiteral("\\b(0[xX][0-9a-fA-F]+|\\-?[0-9]+(\\.[0-9]+)?([eE]\\-?[0-9]+)?)\\b"));
    rule.format = theme.constants();
    rules.append(rule);

    // string literal with double quotes
    rule.pattern = QRegularExpression(QStringLiteral("\".*[^\\\\]\""), QRegularExpression::InvertedGreedinessOption);
    rule.format = theme.stringLiteral();
    rules.append(rule);

    // string literal with single quotes
    rule.pattern = QRegularExpression(QStringLiteral("'.*[^\\\\]'"), QRegularExpression::InvertedGreedinessOption);
    rule.format = theme.stringLiteral();
    rules.append(rule);

    // single-line comments
    rule.pattern = QRegularExpression(QStringLiteral("\\-\\-[^\n]*"));
    rule.format = theme.comment();
    rules.append(rule);
}

void LuaHighlighter::highlightBlock(const QString &text)
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
