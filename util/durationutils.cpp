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

#include "durationutils.h"

QString DurationUtils::durationToMinutesSeconds(const qint64 durationMs)
{
    if (durationMs < 0)
    {
        return "--:--";
    }

    qint64 seconds = durationMs / 1000; // rounded down
    const qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    return QString::number(minutes) + ":"
           + (seconds < 10 ? "0" : "") + QString::number(seconds);
}
