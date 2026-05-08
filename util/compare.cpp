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

#include "compare.h"
#include <algorithm> // for std::max() + std::min()
#include <cstring> // std::memcmp()
#include <fstream>
#include <limits>
#include <QDir>

Compare::Order Compare::order(const QFileInfo &left, const QFileInfo &right)
{
    if (left.isDir() != right.isDir())
    {
        return left.isDir() ? Order::LeftIsFirst : Order::RightIsFirst;
    }

    const int cmp = left.fileName().compare(right.fileName());
    if (cmp < 0)
    {
        return Order::LeftIsFirst;
    }
    if (cmp == 0)
    {
        return Order::Same;
    }
    // cmp > 0
    return Order::RightIsFirst;
}

Compare::Content Compare::contents(const QFileInfo &left, const QFileInfo &right)
{
    std::ifstream streamLeft(left.filesystemAbsoluteFilePath(),
                             std::ios_base::binary | std::ios_base::ate);
    if (streamLeft.fail())
    {
        return Content::Unknown;
    }
    std::ifstream streamRight(right.filesystemAbsoluteFilePath(),
                              std::ios_base::binary | std::ios_base::ate);
    if (streamRight.fail())
    {
        streamLeft.close();
        return Content::Unknown;
    }

    // Check for same size.
    const std::ifstream::pos_type size = streamLeft.tellg();
    if (size != streamRight.tellg())
    {
        return Content::Different;
    }

    streamLeft.seekg(0, std::ifstream::beg);
    streamRight.seekg(0, std::ifstream::beg);

    constexpr std::size_t bufferSize = 4096;
    char bufferLeft[bufferSize];
    char bufferRight[bufferSize];

    std::ifstream::pos_type remaining = size;

    while ((remaining > 0) && !streamLeft.eof() && !streamRight.eof())
    {
        const auto to_read = std::min(remaining, static_cast<std::ifstream::pos_type>(bufferSize));
        streamLeft.read(bufferLeft, to_read);
        streamRight.read(bufferRight, to_read);
        if (!streamLeft.good() || !streamRight.good())
        {
            return Content::Unknown;
        }
        remaining -= to_read;
        if (std::memcmp(bufferLeft, bufferRight, to_read) != 0)
        {
            return Content::Different;
        }
    };

    return Content::Identical;
}

void Compare::compareDirectories(const QString &left, const QString &right)
{
    const QDir::Filters compareFilter = QDir::Filter::AllEntries
                                        | QDir::Filter::NoDotAndDotDot
                                        | QDir::Filter::Hidden
                                        | QDir::Filter::System;
    const QDir::SortFlags compareSort = QDir::SortFlag::Name
                                        | QDir::SortFlag::DirsFirst
                                        | QDir::SortFlag::IgnoreCase;

    QDir leftDir(left);
    QDir rightDir(right);
    if (!leftDir.exists() || !rightDir.exists())
    {
        emit compareFinished({});
        return;
    }

    leftDir.setFilter(compareFilter);
    leftDir.setSorting(compareSort);

    rightDir.setFilter(compareFilter);
    rightDir.setSorting(compareSort);

    const QFileInfoList leftList = leftDir.entryInfoList();
    const QFileInfoList rightList = rightDir.entryInfoList();

    auto leftCurrent = leftList.cbegin();
    auto rightCurrent = rightList.cbegin();

    // progress reset to zero
    int progress = 0;
    emit progressChanged(0);
    // set maximum to leftList.size() + rightList.size()
    const qsizetype maximum_qst = leftList.size() + rightList.size();
    if (maximum_qst > std::numeric_limits<int>::max())
    {
        emit maximumChanged(std::numeric_limits<int>::max());
    }
    else
    {
        emit maximumChanged(maximum_qst);
    }


    QList<Compare::Info> result;
    result.reserve(std::max(leftList.size(), rightList.size()));

    while ((leftCurrent != leftList.cend()) || (rightCurrent != rightList.cend()))
    {
        if (cancellationRequested)
        {
            emit compareCancelled(result);
            return;
        }

        const bool leftFinished = leftCurrent == leftList.cend();
        const bool rightFinished = rightCurrent == rightList.cend();

        if (leftFinished)
        {
            // Entry only exists on right side.
            result.append(rightSideOnly(*rightCurrent));
            ++rightCurrent;
            // progress increased by one
            if (progress < std::numeric_limits<int>::max())
            {
              emit progressChanged(++progress);
            }
        }
        else if (rightFinished)
        {
            // Entry only exists on left side.
            result.append(leftSideOnly(*leftCurrent));
            ++leftCurrent;
            // progress increased by one
            if (progress < std::numeric_limits<int>::max())
            {
                emit progressChanged(++progress);
            }
        }
        else
        {
            // Both sides still have elements.
            const auto order = Compare::order(*leftCurrent, *rightCurrent);
            switch (order) {
            case Compare::Order::LeftIsFirst:
                result.append(leftSideOnly(*leftCurrent));
                ++leftCurrent;
                // progress increased by one
                if (progress < std::numeric_limits<int>::max())
                {
                    emit progressChanged(++progress);
                }
                continue;
                break;
            case Compare::Order::RightIsFirst:
                result.append(rightSideOnly(*rightCurrent));
                ++rightCurrent;
                // progress increased by one
                if (progress < std::numeric_limits<int>::max())
                {
                    emit progressChanged(++progress);
                }
                continue;
                break;
            default:
                break;
            }

            // File name and type (file or directory) is the same on both sides.
            if (leftCurrent->isDir())
            {
                result.append(directoryExists(*leftCurrent, *rightCurrent));
                ++leftCurrent;
                ++rightCurrent;
                // progress increased by two
                if (progress < std::numeric_limits<int>::max() - 1)
                {
                    progress += 2;
                    emit progressChanged(progress);
                }
                continue;
            }

            result.append(fileEntry(*leftCurrent, *rightCurrent));
            ++leftCurrent;
            ++rightCurrent;
            // progress increased by two
            if (progress < std::numeric_limits<int>::max() - 1)
            {
                progress += 2;
                emit progressChanged(progress);
            }
        }
    }

    emit compareFinished(result);
}

void Compare::requestCancellation()
{
    cancellationRequested = true;
}

Compare::Info Compare::leftSideOnly(const QFileInfo &info)
{
    Info result;
    result.name = info.fileName();
    result.isDirectory = info.isDir();
    result.result = Result::LeftSideOnly;
    result.leftDate = info.lastModified();
    result.rightDate = QDateTime();
    result.leftSize = info.size();
    result.rightSize = -1;
    return result;
}

Compare::Info Compare::rightSideOnly(const QFileInfo &info)
{
    Info result;
    result.name = info.fileName();
    result.isDirectory = info.isDir();
    result.result = Result::RightSideOnly;
    result.leftDate = QDateTime();
    result.rightDate = info.lastModified();
    result.leftSize = -1;
    result.rightSize = info.size();
    return result;
}

Compare::Info Compare::directoryExists(const QFileInfo &left, const QFileInfo &right)
{
    Info result;
    result.name = left.fileName();
    result.isDirectory = true;
    result.result = Result::Unknown;
    result.leftDate = left.lastModified();
    result.rightDate = right.lastModified();
    result.leftSize = -1;
    result.rightSize = -1;
    return result;
}

Compare::Info Compare::fileEntry(const QFileInfo &left, const QFileInfo &right)
{
    Info result;
    result.name = left.fileName();
    result.isDirectory = false;
    result.result = static_cast<Result>(static_cast<std::underlying_type_t<Content>>(
        Compare::contents(left, right)));
    result.leftDate = left.lastModified();
    result.rightDate = right.lastModified();
    result.leftSize = left.size();
    result.rightSize = right.size();
    return result;
}
