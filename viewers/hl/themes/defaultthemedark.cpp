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

#include "defaultthemedark.h"

QTextCharFormat DefaultThemeDark::keyword() const
{
    QTextCharFormat keywordFormat;
    keywordFormat.setFontWeight(QFont::Weight::Bold);
    keywordFormat.setForeground(QColor(0xFF, 0x8F, 0x40));
    return keywordFormat;
}

QTextCharFormat DefaultThemeDark::preprocessor() const
{
    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setForeground(QColor(0xF0, 0x71, 0x78));
    return preprocessorFormat;
}

QTextCharFormat DefaultThemeDark::stringLiteral() const
{
    QTextCharFormat stringLiteralFormat;
    stringLiteralFormat.setForeground(QColor(0xAA, 0xD9, 0x4C));
    return stringLiteralFormat;
}

QTextCharFormat DefaultThemeDark::comment() const
{
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(0x5A, 0x66, 0x73));
    return commentFormat;
}
