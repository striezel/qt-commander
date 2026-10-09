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

#include "objectcount.h"
#include <QObject>

ObjectCount::ObjectCount()
    : numDirs(0)
    , numFiles(0)
{}

QString ObjectCount::text() const
{
    const bool oneDir = numDirs == 1;
    const bool oneFile = numFiles == 1;
    const bool oneObject = (numDirs + numFiles) == 1;
    return QObject::tr("%1 %2 (%3 %4, %5 %6)")
        .arg(numDirs + numFiles)
        .arg(oneObject ? QObject::tr("object") : QObject::tr("objects"))
        .arg(numDirs)
        .arg(oneDir ? QObject::tr("directory") : QObject::tr("directories"))
        .arg(numFiles)
        .arg(oneFile ? QObject::tr("file") : QObject::tr("files"));
}

void ObjectCount::decreaseDirectoryCount()
{
    if (numDirs > 0)
    {
        --numDirs;
    }
}

void ObjectCount::decreaseFileCount()
{
    if (numFiles > 0)
    {
        --numFiles;
    }
}
