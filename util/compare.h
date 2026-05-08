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

#ifndef COMPARE_H
#define COMPARE_H

#include <type_traits> // for std::underlying_type_t (C++14)
#include <QFileInfo>
#include <QList>
#include <QObject>
#include <QString>

class Compare: public QObject
{
    Q_OBJECT
public:
    /// Enumeration to indicate relative sort order of left and right tree
    /// entries.
    enum class Order
    {
        LeftIsFirst,
        Same,
        RightIsFirst
    };

    static Order order(const QFileInfo& left, const QFileInfo& right);

    /// Enumeration to indicate the content comparison result between two files.
    enum class Content
    {
        /// Files have the same conent.
        Identical,

        /// Files have different content.
        Different,

        /// Unknown (probably caused by I/O failure on one file read)
        Unknown
    };

    /// Compares the contents of two files.
    static Content contents(const QFileInfo& left, const QFileInfo& right);


    enum class Result
    {
        Identical = static_cast<std::underlying_type_t<Content>>(Content::Identical),
        Different = static_cast<std::underlying_type_t<Content>>(Content::Different),
        Unknown = static_cast<std::underlying_type_t<Content>>(Content::Unknown),
        LeftSideOnly,
        RightSideOnly
    };

    struct Info
    {
        QString name;
        bool isDirectory;
        Result result;
        QDateTime leftDate;
        QDateTime rightDate;
        qint64 leftSize;
        qint64 rightSize;
    };

    void compareDirectories(const QString &left, const QString &right);

signals:
    void progressChanged(int currentProgress);
    void maximumChanged(int maximum);
    void compareFinished(const QList<Info>& list);

private:
    static Info leftSideOnly(const QFileInfo& info);
    static Info rightSideOnly(const QFileInfo& info);
    static Info directoryExists(const QFileInfo& left, const QFileInfo &right);
    static Info fileEntry(const QFileInfo& left, const QFileInfo &right);
};

#endif // COMPARE_H
