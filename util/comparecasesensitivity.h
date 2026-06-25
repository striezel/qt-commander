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

#ifndef COMPARECASESENSITIVITY_H
#define COMPARECASESENSITIVITY_H

#include <Qt>
#include <QDir>

/// Enumeration class that indicates how file names shall be compared.
enum class CompareCaseSensitivity
{
    /// Use the default for the current OS. This means case-insensitive for
    /// Windows and case-sensitive for all other OSes.
    SystemDefault,

    /// Compare file names in case-sensitive way.
    CaseSensitive,

    /// Ignore case when comparing file names.
    CaseInsensitive
};

constexpr Qt::CaseSensitivity toQtCaseSensitivity(const CompareCaseSensitivity ccs)
{
    switch (ccs)
    {
    case CompareCaseSensitivity::CaseInsensitive:
        return Qt::CaseSensitivity::CaseInsensitive;
    case CompareCaseSensitivity::CaseSensitive:
        return Qt::CaseSensitivity::CaseSensitive;
    default:
#if defined(_WIN32)
        return Qt::CaseSensitivity::CaseInsensitive;
#else
        return Qt::CaseSensitivity::CaseSensitive;
#endif
    }
}

constexpr QDir::SortFlag extraSortFlag(const CompareCaseSensitivity ccs)
{
    switch (ccs)
    {
    case CompareCaseSensitivity::CaseInsensitive:
        return QDir::SortFlag::IgnoreCase;
    case CompareCaseSensitivity::CaseSensitive:
        return static_cast<QDir::SortFlag>(0x00);
    default:
#if defined(_WIN32)
        return QDir::SortFlag::IgnoreCase;
#else
        return static_cast<QDir::SortFlag>(0x00);
#endif
    }
}

#endif // COMPARECASESENSITIVITY_H
