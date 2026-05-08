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

#include "directorycomparewindow.h"
#include "ui_directorycomparewindow.h"

#include <QtConcurrent/QtConcurrentTask>
#include <QDir>
#include <QFileIconProvider>
#include <QStringList>
#include <QTreeWidgetItem>

DirectoryCompareWindow::DirectoryCompareWindow(const QString& pathLeft, const QString& pathRight, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DirectoryCompareWindow)
    , initialShowDone(false)
    , leftPath(pathLeft)
    , rightPath(pathRight)
{
    ui->setupUi(this);

    connect(ui->actionClose, &QAction::triggered, this, &DirectoryCompareWindow::close);
}

DirectoryCompareWindow::~DirectoryCompareWindow()
{
    delete ui;
}

void DirectoryCompareWindow::closeEvent(QCloseEvent *event)
{
    // do the usual event handling inherited from base class
    this->QMainWindow::closeEvent(event);

    // ensure deletion
    this->deleteLater();
}

void DirectoryCompareWindow::showEvent(QShowEvent *event)
{
    if (initialShowDone)
    {
        return;
    }

    initialShowDone = true;
    moveToThread(&thread);
    connect(&thread, &QThread::started, this, &DirectoryCompareWindow::startCompare);
    thread.start();
}

void DirectoryCompareWindow::startCompare()
{
    compareDirectories(leftPath, rightPath);
}

void DirectoryCompareWindow::compareDirectories(const QString &left, const QString &right)
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
        return;
    }

    ui->lineEditLeft->setText(left);
    ui->lineEditRight->setText(right);

    leftDir.setFilter(compareFilter);
    leftDir.setSorting(compareSort);

    rightDir.setFilter(compareFilter);
    rightDir.setSorting(compareSort);

    const QLocale loc = locale();

    const QFileInfoList leftList = leftDir.entryInfoList();
    const QFileInfoList rightList = rightDir.entryInfoList();

    auto leftCurrent = leftList.cbegin();
    auto rightCurrent = rightList.cbegin();

    ui->treeWidget->clear();
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(leftList.size() + rightList.size());

    while ((leftCurrent != leftList.cend()) || (rightCurrent != rightList.cend()))
    {
        const bool leftFinished = leftCurrent == leftList.cend();
        const bool rightFinished = rightCurrent == rightList.cend();

        if (leftFinished)
        {
            // Entry only exists on right side.
            addRightSideOnlyEntry(*rightCurrent, loc);
            ++rightCurrent;
            ui->progressBar->setValue(ui->progressBar->value() + 1);
        }
        else if (rightFinished)
        {
            // Entry only exists on left side.
            addLeftSideOnlyEntry(*leftCurrent, loc);
            ++leftCurrent;
            ui->progressBar->setValue(ui->progressBar->value() + 1);
        }
        else
        {
            // Both sides still have elements.
            const auto order = Compare::order(*leftCurrent, *rightCurrent);
            switch (order) {
            case Compare::Order::LeftIsFirst:
                addLeftSideOnlyEntry(*leftCurrent, loc);
                ++leftCurrent;
                ui->progressBar->setValue(ui->progressBar->value() + 1);
                continue;
                break;
            case Compare::Order::RightIsFirst:
                addRightSideOnlyEntry(*rightCurrent, loc);
                ++rightCurrent;
                ui->progressBar->setValue(ui->progressBar->value() + 1);
                continue;
                break;
            default:
                break;
            }

            // File name and type (file or directory) is the same on both sides.
            if (leftCurrent->isDir())
            {
                addDirectoryExistsEntry(*leftCurrent, *rightCurrent, loc);
                ++leftCurrent;
                ++rightCurrent;
                ui->progressBar->setValue(ui->progressBar->value() + 2);
                continue;
            }

            const auto content = Compare::contents(*leftCurrent, *rightCurrent);
            addFileEntry(*leftCurrent, *rightCurrent, loc, content);
            ++leftCurrent;
            ++rightCurrent;
            ui->progressBar->setValue(ui->progressBar->value() + 2);
        }
    }

    // Adjust width of columns with modification date.
    ui->treeWidget->resizeColumnToContents(2);
    ui->treeWidget->resizeColumnToContents(3);
    // Adjust width of columns with file sizes.
    ui->treeWidget->resizeColumnToContents(4);
    ui->treeWidget->resizeColumnToContents(5);

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);

    thread.exit();
}

void DirectoryCompareWindow::addLeftSideOnlyEntry(const QFileInfo &info, const QLocale& loc)
{
    QStringList data;
    data.append(info.fileName());
    data.append("Nur links: " + info.absolutePath());
    data.append(loc.toString(info.lastModified(), QLocale::NarrowFormat));
    data.append("keins");
    if (info.isDir())
    {
        data << "Verzeichnis" << "keine";
    }
    else
    {
        data.append(loc.formattedDataSize(info.size()));
        data.append("keine");
    }
    QTreeWidgetItem* item = new QTreeWidgetItem(data);
    const QFileIconProvider icon_provider;
    item->setIcon(0, info.isDir()
                         ? icon_provider.icon(QAbstractFileIconProvider::Folder)
                         : icon_provider.icon(QAbstractFileIconProvider::File));
    item->setIcon(1, QIcon::fromTheme("go-previous"));
    ui->treeWidget->addTopLevelItem(item);
}

void DirectoryCompareWindow::addLeftSideOnlyEntry(const Compare::Info &info, const QLocale &loc)
{
    QStringList data;
    data.append(info.name);
    data.append("Nur links: " + info.name);
    data.append(loc.toString(info.leftDate, QLocale::NarrowFormat));
    data.append("keins");
    if (info.isDirectory)
    {
        data << "Verzeichnis" << "keine";
    }
    else
    {
        data.append(loc.formattedDataSize(info.leftSize));
        data.append("keine");
    }
    QTreeWidgetItem* item = new QTreeWidgetItem(data);
    const QFileIconProvider icon_provider;
    item->setIcon(0, info.isDirectory
                         ? icon_provider.icon(QAbstractFileIconProvider::Folder)
                         : icon_provider.icon(QAbstractFileIconProvider::File));
    item->setIcon(1, QIcon::fromTheme("go-previous"));
    ui->treeWidget->addTopLevelItem(item);
}

void DirectoryCompareWindow::addRightSideOnlyEntry(const QFileInfo &info, const QLocale& loc)
{
    QStringList data;
    data.append(info.fileName());
    data.append("Nur rechts: " + info.absolutePath());
    data.append("keins");
    data.append(loc.toString(info.lastModified(), QLocale::NarrowFormat));
    if (info.isDir())
    {
        data << "keine" << "Verzeichnis";
    }
    else
    {
        data.append("keine");
        data.append(loc.formattedDataSize(info.size()));
    }
    QTreeWidgetItem* item = new QTreeWidgetItem(data);
    const QFileIconProvider icon_provider;
    item->setIcon(0, info.isDir()
                         ? icon_provider.icon(QAbstractFileIconProvider::Folder)
                         : icon_provider.icon(QAbstractFileIconProvider::File));
    item->setIcon(1, QIcon::fromTheme("go-next"));
    ui->treeWidget->addTopLevelItem(item);
}

void DirectoryCompareWindow::addRightSideOnlyEntry(const Compare::Info &info, const QLocale &loc)
{
    QStringList data;
    data.append(info.name);
    data.append("Nur rechts: " + info.name);
    data.append("keins");
    data.append(loc.toString(info.rightDate, QLocale::NarrowFormat));
    if (info.isDirectory)
    {
        data << "keine" << "Verzeichnis";
    }
    else
    {
        data.append("keine");
        data.append(loc.formattedDataSize(info.rightSize));
    }
    QTreeWidgetItem* item = new QTreeWidgetItem(data);
    const QFileIconProvider icon_provider;
    item->setIcon(0, info.isDirectory
                         ? icon_provider.icon(QAbstractFileIconProvider::Folder)
                         : icon_provider.icon(QAbstractFileIconProvider::File));
    item->setIcon(1, QIcon::fromTheme("go-next"));
    ui->treeWidget->addTopLevelItem(item);
}

void DirectoryCompareWindow::addDirectoryExistsEntry(const QFileInfo &left, const QFileInfo &right, const QLocale &loc)
{
    QStringList data;
    data.append(left.fileName());
    data.append("Existiert in beiden Verzeichnissen");
    data.append(loc.toString(left.lastModified(), QLocale::NarrowFormat));
    data.append(loc.toString(right.lastModified(), QLocale::NarrowFormat));
    data << "Verzeichnis" << "Verzeichnis";

    QTreeWidgetItem* item = new QTreeWidgetItem(data);

    const QFileIconProvider icon_provider;
    item->setIcon(0, icon_provider.icon(QAbstractFileIconProvider::Folder));

    // Subdirectories are not checked yet, so status is ... questionable / unknown.
    item->setIcon(1, QIcon::fromTheme("dialog-question"));

    ui->treeWidget->addTopLevelItem(item);
}

void DirectoryCompareWindow::addDirectoryExistsEntry(const Compare::Info &info, const QLocale &loc)
{
    QStringList data;
    data.append(info.name);
    data.append("Existiert in beiden Verzeichnissen");
    data.append(loc.toString(info.leftDate, QLocale::NarrowFormat));
    data.append(loc.toString(info.rightDate, QLocale::NarrowFormat));
    data << "Verzeichnis" << "Verzeichnis";

    QTreeWidgetItem* item = new QTreeWidgetItem(data);

    const QFileIconProvider icon_provider;
    item->setIcon(0, icon_provider.icon(QAbstractFileIconProvider::Folder));

    // Subdirectories are not checked yet, so status is ... questionable / unknown.
    item->setIcon(1, QIcon::fromTheme("dialog-question"));

    ui->treeWidget->addTopLevelItem(item);
}

void DirectoryCompareWindow::addFileEntry(const QFileInfo &left, const QFileInfo &right, const QLocale &loc, const Compare::Content content)
{
    QStringList data;
    data.append(left.fileName());
    switch(content)
    {
    case Compare::Content::Identical:
        data.append("Dateien sind identisch.");
        break;
    case Compare::Content::Different:
        data.append("Dateien sind unterschiedlich.");
        break;
    case Compare::Content::Unknown:
        data.append("Dateien konnten nicht verglichen werden.");
        break;
    }
    data.append(loc.toString(left.lastModified(), QLocale::NarrowFormat));
    data.append(loc.toString(right.lastModified(), QLocale::NarrowFormat));
    data.append(loc.formattedDataSize(left.size()));
    data.append(loc.formattedDataSize(right.size()));

    QTreeWidgetItem* item = new QTreeWidgetItem(data);
    QIcon icon;
    switch(content)
    {
    case Compare::Content::Identical:
        icon = QIcon::fromTheme("document-new");
        break;
    case Compare::Content::Different:
        icon = QIcon::fromTheme("edit-copy");
        break;
    case Compare::Content::Unknown:
        icon = QIcon::fromTheme("dialog-question");
        break;
    }
    if (!icon.isNull())
    {
        item->setIcon(1, icon);
    }

    const QFileIconProvider icon_provider;
    icon = icon_provider.icon(QAbstractFileIconProvider::File);
    item->setIcon(0, icon);

    ui->treeWidget->addTopLevelItem(item);
}

void DirectoryCompareWindow::addFileEntry(const Compare::Info &info, const QLocale &loc)
{
    QStringList data;
    data.append(info.name);
    switch(info.result)
    {
    case Compare::Result::Identical:
        data.append("Dateien sind identisch.");
        break;
    case Compare::Result::Different:
        data.append("Dateien sind unterschiedlich.");
        break;
    case Compare::Result::Unknown:
        data.append("Dateien konnten nicht verglichen werden.");
        break;
    default:
        // Should not happen, but intercept it anyway.
        data.append("Fehler: Wert " + QString::number(static_cast<int>(info.result)));
        break;
    }
    data.append(loc.toString(info.leftDate, QLocale::NarrowFormat));
    data.append(loc.toString(info.rightDate, QLocale::NarrowFormat));
    data.append(loc.formattedDataSize(info.leftSize));
    data.append(loc.formattedDataSize(info.rightSize));

    QTreeWidgetItem* item = new QTreeWidgetItem(data);
    QIcon icon;
    switch(info.result)
    {
    case Compare::Result::Identical:
        icon = QIcon::fromTheme("document-new");
        break;
    case Compare::Result::Different:
        icon = QIcon::fromTheme("edit-copy");
        break;
    case Compare::Result::Unknown:
        icon = QIcon::fromTheme("dialog-question");
        break;
    }
    if (!icon.isNull())
    {
        item->setIcon(1, icon);
    }

    const QFileIconProvider icon_provider;
    icon = icon_provider.icon(QAbstractFileIconProvider::File);
    item->setIcon(0, icon);

    ui->treeWidget->addTopLevelItem(item);
}

void DirectoryCompareWindow::addInfoEntry(const Compare::Info &info, const QLocale &loc)
{
    if (info.result == Compare::Result::LeftSideOnly)
    {
        addLeftSideOnlyEntry(info, loc);
    }
    else if (info.result == Compare::Result::LeftSideOnly)
    {
        addRightSideOnlyEntry(info, loc);
    }
    else if (info.isDirectory)
    {
        addDirectoryExistsEntry(info, loc);
    }
    else
    {
        addFileEntry(info, loc);
    }
}
