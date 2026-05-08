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

#ifndef DIRECTORYCOMPAREWINDOW_H
#define DIRECTORYCOMPAREWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QFileInfo>
#include <QString>
#include <QThread>

#include "util/compare.h"

namespace Ui {
class DirectoryCompareWindow;
}

class DirectoryCompareWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DirectoryCompareWindow(const QString& pathLeft, const QString& pathRight, QWidget *parent = nullptr);
    ~DirectoryCompareWindow();

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;

private slots:
    void startCompare();
private:
    Ui::DirectoryCompareWindow *ui;

    bool initialShowDone;
    QString leftPath;
    QString rightPath;
    QThread thread;

    void compareDirectories(const QString& left, const QString& right);

    void addLeftSideOnlyEntry(const QFileInfo& info, const QLocale& loc);
    void addRightSideOnlyEntry(const QFileInfo& info, const QLocale& loc);
    void addDirectoryExistsEntry(const QFileInfo& left, const QFileInfo& right, const QLocale& loc);
    void addFileEntry(const QFileInfo& left, const QFileInfo& right, const QLocale& loc, const Compare::Content content);

    void addInfoEntry(const Compare::Info& info, const QLocale& loc);
    void addLeftSideOnlyEntry(const Compare::Info& info, const QLocale& loc);
    void addRightSideOnlyEntry(const Compare::Info& info, const QLocale& loc);
    void addDirectoryExistsEntry(const Compare::Info& info, const QLocale& loc);
    void addFileEntry(const Compare::Info& info, const QLocale& loc);
};

#endif // DIRECTORYCOMPAREWINDOW_H
