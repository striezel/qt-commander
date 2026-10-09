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

#ifndef OBJECTCOUNT_H
#define OBJECTCOUNT_H

#include <qglobal.h>

class ObjectCount
{
public:
    ObjectCount();

    quint64 numDirs;
    quint64 numFiles;

    /// Gets a text that contains object count (directories + files), directory
    /// count and file count and is suitable to be displayed to the user.
    QString text() const;

    /// Decreases number of directories by one.
    void decreaseDirectoryCount();

    /// Decreases number of files by one.
    void decreaseFileCount();
};

#endif // OBJECTCOUNT_H
