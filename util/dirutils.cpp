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

#include "dirutils.h"

#include <QFile>
#include <QString>

bool DirUtils::isParentOf(const QDir& parent, const QDir& potentialChild)
{
    QString parentPath = parent.canonicalPath();
    if (parentPath.isEmpty())
    {
        // Canonical path can be an empty string, if the path contains
        // unresolvable symlinks.
        parentPath = parent.absolutePath();
    }

    QString childPath = potentialChild.canonicalPath();
    if (childPath.isEmpty())
    {
        childPath = potentialChild.absolutePath();
    }

#ifdef _WIN32
    // QDir seems to use '/' as separator on Windows, too, so let's check that,
    // too, even if we are on Windows.
    const QChar alternativeSeparator = '/';
#endif
    return (parentPath == childPath)
#ifdef _WIN32
           || childPath.startsWith(parentPath + alternativeSeparator)
#endif
           || childPath.startsWith(parentPath + QDir::separator());
}

bool DirUtils::isSameDir(const QDir &one, const QDir &two)
{
    QString firstPath = one.canonicalPath();
    if (firstPath.isEmpty())
    {
        // Canonical path can be an empty string, if the path contains
        // unresolvable symlinks.
        firstPath = one.absolutePath();
    }

    QString secondPath = two.canonicalPath();
    if (secondPath.isEmpty())
    {
        secondPath = two.absolutePath();
    }

    return firstPath == secondPath;
}

bool DirUtils::copyRecursively(const QDir &source, const QDir &destination)
{
    if (!source.exists())
    {
        return false;
    }

    if (destination.exists())
    {
        return false;
    }

    if (!destination.mkdir("."))
    {
        return false;
    }

    const QFileInfoList list = source.entryInfoList(
        QDir::Filter::AllEntries | QDir::Filter::NoDotAndDotDot | QDir::Filter::Hidden);

    for (const QFileInfo& info: list)
    {
        if (!info.isDir())
        {
            if (!QFile::copy(source.absoluteFilePath(info.fileName()), destination.absoluteFilePath(info.fileName())))
            {
                return false;
            }
        }
        else
        {
            // Directory has to be handled recursively.
            return copyRecursively(source.absoluteFilePath(info.fileName()),
                                   destination.absoluteFilePath(info.fileName()));
        }
    }

    // Done, all elements were copied.
    return true;
}
