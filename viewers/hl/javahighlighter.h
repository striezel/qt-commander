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

#ifndef JAVAHIGHLIGHTER_H
#define JAVAHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include "highlighterrule.h"
#include "themes/theme.h"

class JavaHighlighter: public QSyntaxHighlighter
{
public:
    JavaHighlighter(const Theme& theme, QTextDocument* parent = nullptr);
protected:
    void highlightBlock(const QString &text) override;
private:
    QList<HighlighterRule> rules;

    const QTextCharFormat commentFormat;

    static constexpr int NotInMultiLineCommentState = 0;
    static constexpr int IsInMultiLineCommentState = 1;
};

#endif // JAVAHIGHLIGHTER_H
