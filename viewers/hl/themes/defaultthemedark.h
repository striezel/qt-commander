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

#ifndef DEFAULTTHEMEDARK_H
#define DEFAULTTHEMEDARK_H

#include "theme.h"

class DefaultThemeDark: public Theme
{
public:
    virtual QTextCharFormat keyword() const override;
    virtual QTextCharFormat preprocessor() const override;
    virtual QTextCharFormat stringLiteral() const override;
    virtual QTextCharFormat comment() const override;
};

#endif // DEFAULTTHEMEDARK_H
