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

#ifndef THEME_H
#define THEME_H

#include <QTextCharFormat>
#include "themeid.h"

class Theme
{
public:
    virtual ~Theme() = default;

    /// Gets the text format for keywords.
    virtual QTextCharFormat keyword() const = 0;

    /// Gets the text format for preprocessor tokens.
    virtual QTextCharFormat preprocessor() const = 0;

    /// Gets the text format for string literals.
    virtual QTextCharFormat stringLiteral() const = 0;

    /// Gets the text format for comments.
    virtual QTextCharFormat comment() const = 0;

    /// Gets the text format for numeric constants.
    virtual QTextCharFormat constants() const = 0;

    /// Gets the text format for operators.
    virtual QTextCharFormat operators() const = 0;

    /// Gets the text format for special expressions like macros.
    virtual QTextCharFormat special() const = 0;

    /// enumeration value to identify the theme
    virtual ThemeId id() const = 0;
};

#endif // THEME_H
