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
#include "mainwindow.h"
#include "ui_directorycomparewindow.h"

#include <QDir>
#include <QMessageBox>
#include <QStringList>
#include <QTreeWidgetItem>

const QString DirectoryCompareWindow::iconNameIdentical{"document-new"};
const QString DirectoryCompareWindow::iconNameDifferent{"edit-copy"};
const QString DirectoryCompareWindow::iconNameUnknown{"dialog-question"};
const QString DirectoryCompareWindow::iconNameLeftSideOnly{"go-previous"};
const QString DirectoryCompareWindow::iconNameRightSideOnly{"go-next"};

DirectoryCompareWindow::DirectoryCompareWindow(const QString& pathLeft, const QString& pathRight, const CompareCaseSensitivity ccs, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DirectoryCompareWindow)
    , initialShowDone(false)
    , leftPath(pathLeft)
    , rightPath(pathRight)
    , compareCaseSensitivity(ccs)
    , compare(nullptr)
    , leftSideChanged(false)
    , rightSideChanged(false)
{
    ui->setupUi(this);

    ui->lineEditLeft->setText(leftPath);
    ui->lineEditRight->setText(rightPath);

    connect(ui->actionClose, &QAction::triggered, this, &DirectoryCompareWindow::threadHandlingClose);
    connect(ui->btnCancel, &QPushButton::clicked, this, &DirectoryCompareWindow::btnCancelClicked);

    connect(ui->actionGoToNextDifference, &QAction::triggered, this, &DirectoryCompareWindow::actionGoToNextDifferenceTriggered);
    connect(ui->actionGoToPrevDifference, &QAction::triggered, this, &DirectoryCompareWindow::actionGoToPrevDifferenceTriggered);
    connect(ui->actionCopyToLeft, &QAction::triggered, this, &DirectoryCompareWindow::actionCopyToLeftTriggered);
    connect(ui->actionCopyToRight, &QAction::triggered, this, &DirectoryCompareWindow::actionCopyToRightTriggered);
    connect(ui->actionDelete, &QAction::triggered, this, &DirectoryCompareWindow::actionDeleteTriggered);
    connect(ui->treeWidget, &QTreeWidget::itemSelectionChanged, this, &DirectoryCompareWindow::treeWidgetSelectionChanged);
}

DirectoryCompareWindow::~DirectoryCompareWindow()
{
    if (leftTreeNeedsUpdate() || rightTreeNeedsUpdate())
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent());
        if (castedParent != nullptr)
        {
            if (leftTreeNeedsUpdate())
            {
                castedParent->changeLeftTree(leftPath);
            }
            if (rightTreeNeedsUpdate())
            {
                castedParent->changeRightTree(rightPath);
            }
        }
    }

    delete ui;
}

bool DirectoryCompareWindow::leftTreeNeedsUpdate() const
{
    return leftSideChanged;
}

bool DirectoryCompareWindow::rightTreeNeedsUpdate() const
{
    return rightSideChanged;
}

void DirectoryCompareWindow::closeEvent(QCloseEvent *event)
{
    thread.quit();
    thread.wait(250);

    // do the usual event handling inherited from base class
    this->QMainWindow::closeEvent(event);

    // ensure deletion
    this->deleteLater();
}

void DirectoryCompareWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);

    if (initialShowDone)
    {
        return;
    }

    initialShowDone = true;

    compare = new Compare;
    compare->moveToThread(&thread);

    connect(&thread, &QThread::finished, compare, &QObject::deleteLater);
    connect(this, &DirectoryCompareWindow::compareDirectories, compare, &Compare::compareDirectories);
    connect(compare, &Compare::progressChanged, this, &DirectoryCompareWindow::progressChanged);
    connect(compare, &Compare::maximumChanged, this, &DirectoryCompareWindow::progressMaximumChanged);
    connect(compare, &Compare::compareFinished, this, &DirectoryCompareWindow::compareFinished);
    connect(compare, &Compare::compareCancelled, this, &DirectoryCompareWindow::compareCancelled);

    thread.start();

    compareDirectories(leftPath, rightPath, compareCaseSensitivity);
}

void DirectoryCompareWindow::progressChanged(int currentProgress)
{
    if (currentProgress < 0)
    {
        return;
    }

    ui->progressBar->setValue(currentProgress);
}

void DirectoryCompareWindow::progressMaximumChanged(int maximum)
{
    if (maximum <= 0)
    {
        maximum = 1;
    }

    ui->progressBar->setMaximum(maximum);
}

void DirectoryCompareWindow::compareFinished(const QList<Compare::Info>& list)
{
    statusBar()->showMessage(tr("The comparison is complete."));
    addResult(list);
}

void DirectoryCompareWindow::addResult(const QList<Compare::Info>& list)
{
    const QLocale loc = locale();
    ui->treeWidget->clear();
    for (const Compare::Info& info : list)
    {
        addInfoEntry(info, loc);
    }

    // Adjust width of columns with modification date.
    ui->treeWidget->resizeColumnToContents(colIdxLeftDate);
    ui->treeWidget->resizeColumnToContents(colIdxRightDate);
    // Adjust width of columns with file sizes.
    ui->treeWidget->resizeColumnToContents(colIdxLeftSize);
    ui->treeWidget->resizeColumnToContents(colIdxRightSize);

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(colIdxResult, QHeaderView::ResizeMode::Stretch);

    ui->btnCancel->setEnabled(false);
}

void DirectoryCompareWindow::compareCancelled(const QList<Compare::Info> &list)
{
    statusBar()->showMessage(tr("Comparison has been cancelled."));
    addResult(list);
}

void DirectoryCompareWindow::threadHandlingClose()
{
    thread.quit();
    if (!thread.wait(250))
    {
        thread.terminate();
        thread.wait();
    }
    (void) this->close();
}

void DirectoryCompareWindow::btnCancelClicked()
{
    if (compare != nullptr)
    {
        compare->requestCancellation();
    }
    ui->btnCancel->setEnabled(false);
}

void DirectoryCompareWindow::treeWidgetSelectionChanged()
{
    const QList<QTreeWidgetItem*> selection = ui->treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        ui->actionCopyToLeft->setEnabled(false);
        ui->actionCopyToRight->setEnabled(false);
        ui->actionDelete->setEnabled(false);
        return;
    }

    const QTreeWidgetItem* item = selection.at(0);
    const Compare::Info info = item->data(colIdxResult, Qt::UserRole).value<Compare::Info>();
    ui->actionCopyToLeft->setEnabled(
        (info.result == Compare::Result::RightSideOnly)
        || (info.result == Compare::Result::Different));
    ui->actionCopyToRight->setEnabled(
        (info.result == Compare::Result::LeftSideOnly)
        || (info.result == Compare::Result::Different));
    ui->actionDelete->setEnabled(!info.isDirectory);
}

void DirectoryCompareWindow::actionGoToNextDifferenceTriggered()
{
    if (ui->treeWidget->topLevelItemCount() == 0)
    {
        return;
    }

    const QList<QTreeWidgetItem*> selection = ui->treeWidget->selectedItems();
    QTreeWidgetItem* item = nullptr;
    if (selection.isEmpty())
    {
        item = ui->treeWidget->topLevelItem(0);
        const Compare::Info info = item->data(colIdxResult, Qt::UserRole).value<Compare::Info>();
        if ((info.result == Compare::Result::Different)
            || (info.result == Compare::Result::LeftSideOnly)
            || (info.result == Compare::Result::RightSideOnly))
        {
            item->setSelected(true);
            ui->treeWidget->scrollToItem(item);
            return;
        }
    }
    else
    {
        item = selection.at(0);
    }

    int currentIndex = ui->treeWidget->indexOfTopLevelItem(item);
    while (currentIndex < ui->treeWidget->topLevelItemCount() - 1)
    {
        currentIndex += 1;
        item = ui->treeWidget->topLevelItem(currentIndex);
        const Compare::Info info = item->data(colIdxResult, Qt::UserRole).value<Compare::Info>();
        if ((info.result == Compare::Result::Different)
            || (info.result == Compare::Result::LeftSideOnly)
            || (info.result == Compare::Result::RightSideOnly))
        {
            ui->treeWidget->clearSelection();
            item->setSelected(true);
            ui->treeWidget->scrollToItem(item);
            return;
        }
    }

    QMessageBox::information(
        this, tr("No further differences exist"),
        tr("There are no subsequent entries with different files."));
}

void DirectoryCompareWindow::actionGoToPrevDifferenceTriggered()
{
    if (ui->treeWidget->topLevelItemCount() == 0)
    {
        return;
    }

    const QList<QTreeWidgetItem*> selection = ui->treeWidget->selectedItems();
    QTreeWidgetItem* item = nullptr;
    if (selection.isEmpty())
    {
        item = ui->treeWidget->topLevelItem(ui->treeWidget->topLevelItemCount() - 1);
        const Compare::Info info = item->data(colIdxResult, Qt::UserRole).value<Compare::Info>();
        if ((info.result == Compare::Result::Different)
            || (info.result == Compare::Result::LeftSideOnly)
            || (info.result == Compare::Result::RightSideOnly))
        {
            item->setSelected(true);
            ui->treeWidget->scrollToItem(item);
            return;
        }
    }
    else
    {
        item = selection.at(0);
    }

    int currentIndex = ui->treeWidget->indexOfTopLevelItem(item);
    while (currentIndex > 0)
    {
        currentIndex -= 1;
        item = ui->treeWidget->topLevelItem(currentIndex);
        const Compare::Info info = item->data(colIdxResult, Qt::UserRole).value<Compare::Info>();
        if ((info.result == Compare::Result::Different)
            || (info.result == Compare::Result::LeftSideOnly)
            || (info.result == Compare::Result::RightSideOnly))
        {
            ui->treeWidget->clearSelection();
            item->setSelected(true);
            ui->treeWidget->scrollToItem(item);
            return;
        }
    }

    QMessageBox::information(
        this, tr("No prior differences exist"),
        tr("There are no previous entries with different files."));
}

void DirectoryCompareWindow::actionCopyToLeftTriggered()
{
    const QList<QTreeWidgetItem*> selection = ui->treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        QMessageBox::warning(this, tr("No file selected"),
                             tr("No file was selected to copy."));
        return;
    }
    QTreeWidgetItem* item = selection.at(0);
    Compare::Info info = item->data(colIdxResult, Qt::UserRole).value<Compare::Info>();
    if (info.result == Compare::Result::Identical)
    {
        QMessageBox::warning(
            this, tr("Copy not necessary"),
            tr("The file is identical in both directories. Therefore, it is not necessary to copy it."));
        return;
    }
    if (info.result == Compare::Result::LeftSideOnly)
    {
        QMessageBox::warning(
            this, tr("Copy not possible"),
            tr("The file only exists on the left side. ")
                + tr("Therefore, a copy from the right side to the left side is not possible."));
        return;
    }
    if (info.isDirectory)
    {
        QMessageBox::warning(
            this, tr("Copying directories"),
            tr("The selected entry is a directory. ")
                + tr("However, copying directories is not possible in this dialog."));
        return;
    }

    const QString name = info.name;
    const QString source = QDir(rightPath).absoluteFilePath(name);
    const QString destination = QDir(leftPath).absoluteFilePath(name);

    if (QFile::exists(destination))
    {
        const QMessageBox::StandardButton button = QMessageBox::question(
            this, tr("Destination will be overwritten"),
            tr("This action will delete / overwrite the file on the left side. Do you still want to proceed?"));
        if (button != QMessageBox::StandardButton::Yes)
        {
            return;
        }

        if (!QFile::remove(destination))
        {
            QMessageBox::critical(
                this, tr("Error while overwriting"),
                tr("The existing file on the left side could not be removed / overwritten."));
            return;
        }
        leftSideChanged = true;

        info.result = Compare::Result::RightSideOnly;
        info.leftDate = QDateTime();
        info.leftSize = -1;
        item->setIcon(colIdxResult, QIcon::fromTheme(iconNameRightSideOnly));
        item->setData(colIdxResult, Qt::UserRole, QVariant::fromValue(info));
        item->setText(colIdxLeftDate, tr("none", "no_date"));
        item->setText(colIdxLeftSize, tr("none", "no_size"));
    }

    if (!QFile::copy(source, destination))
    {
        QMessageBox::critical(
            this, tr("Copying failed"),
            tr("The file %1 could not be copied to %2.")
                .arg(name)
                .arg(destination));

        // update enabled flag of copy to left/right actions
        treeWidgetSelectionChanged();

        return;
    }
    leftSideChanged = true;

    // Adjust widget item.
    item->setIcon(colIdxResult, QIcon::fromTheme(iconNameIdentical));
    item->setText(colIdxResult, tr("Files are identical."));
    info.result = Compare::Result::Identical;
    const QFileInfo fileInfo = QFileInfo(destination);
    info.leftDate = fileInfo.lastModified();
    info.leftSize = fileInfo.size();
    const QLocale loc = locale();
    item->setText(colIdxLeftDate, loc.toString(info.leftDate, QLocale::NarrowFormat));
    item->setText(colIdxLeftSize, loc.formattedDataSize(info.leftSize));
    item->setData(colIdxResult, Qt::UserRole, QVariant::fromValue(info));

    treeWidgetSelectionChanged();
}

void DirectoryCompareWindow::actionCopyToRightTriggered()
{
    const QList<QTreeWidgetItem*> selection = ui->treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        QMessageBox::warning(this, tr("No file selected"),
                             tr("No file was selected to copy."));
        return;
    }
    QTreeWidgetItem* item = selection.at(0);
    Compare::Info info = item->data(colIdxResult, Qt::UserRole).value<Compare::Info>();
    if (info.result == Compare::Result::Identical)
    {
        QMessageBox::warning(
            this, tr("Copy not necessary"),
            tr("The file is identical in both directories. Therefore, it is not necessary to copy it."));
        return;
    }
    if (info.result == Compare::Result::RightSideOnly)
    {
        QMessageBox::warning(
            this, tr("Copy not possible"),
            tr("The file only exists on the right side. ")
                + tr("Therefore, a copy from the left side to the right side is not possible."));
        return;
    }
    if (info.isDirectory)
    {
        QMessageBox::warning(
            this, tr("Copying directories"),
            tr("The selected entry is a directory. ")
                + tr("However, copying directories is not possible in this dialog."));
        return;
    }

    const QString name = info.name;
    const QString source = QDir(leftPath).absoluteFilePath(name);
    const QString destination = QDir(rightPath).absoluteFilePath(name);

    if (QFile::exists(destination))
    {
        const QMessageBox::StandardButton button = QMessageBox::question(
            this, tr("Destination will be overwritten"),
            tr("This action will delete / overwrite the file on the right side. Do you still want to proceed?"));
        if (button != QMessageBox::StandardButton::Yes)
        {
            return;
        }

        if (!QFile::remove(destination))
        {
            QMessageBox::critical(
                this, tr("Error while overwriting"),
                tr("The existing file on the right side could not be removed / overwritten."));
            return;
        }
        rightSideChanged = true;

        info.result = Compare::Result::LeftSideOnly;
        info.rightDate = QDateTime();
        info.rightSize = -1;
        item->setIcon(colIdxResult, QIcon::fromTheme(iconNameLeftSideOnly));
        item->setData(colIdxResult, Qt::UserRole, QVariant::fromValue(info));
        item->setText(colIdxRightDate, tr("none", "no_date"));
        item->setText(colIdxRightSize, tr("none", "no_size"));
    }

    if (!QFile::copy(source, destination))
    {
        QMessageBox::critical(
            this, tr("Copying failed"),
            tr("The file %1 could not be copied to %2.")
                .arg(name)
                .arg(destination));

        // update enabled flag of copy to left/right actions
        treeWidgetSelectionChanged();

        return;
    }
    rightSideChanged = true;

    // Adjust widget item.
    item->setIcon(colIdxResult, QIcon::fromTheme(iconNameIdentical));
    item->setText(colIdxResult, tr("Files are identical."));
    info.result = Compare::Result::Identical;
    const QFileInfo fileInfo = QFileInfo(destination);
    info.rightDate = fileInfo.lastModified();
    info.rightSize = fileInfo.size();
    const QLocale loc = locale();
    item->setText(colIdxRightDate, loc.toString(info.rightDate, QLocale::NarrowFormat));
    item->setText(colIdxRightSize, loc.formattedDataSize(info.rightSize));
    item->setData(colIdxResult, Qt::UserRole, QVariant::fromValue(info));

    treeWidgetSelectionChanged();
}

void DirectoryCompareWindow::actionDeleteTriggered()
{
    const QList<QTreeWidgetItem*> selection = ui->treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        QMessageBox::warning(this, tr("No file selected"),
                             tr("No file was selected for deletion."));
        return;
    }
    QTreeWidgetItem* item = selection.at(0);
    Compare::Info info = item->data(colIdxResult, Qt::UserRole).value<Compare::Info>();

    if (info.isDirectory)
    {
        QMessageBox::warning(
            this, tr("Deleting directories"),
            tr("The selected entry is a directory. ")
                + tr("However, deleting directories is not possible in this dialog."));
        return;
    }

    {
        QString message;
        switch (info.result)
        {
        case Compare::Result::LeftSideOnly:
            message = tr("This action will delete the file %1 on the left side. Are you sure you want to perform this action?")
                          .arg(info.name);
            break;
        case Compare::Result::RightSideOnly:
            message = tr("This action will delete the file %1 on the right side. Are you sure you want to perform this action?")
                          .arg(info.name);
            break;
        default:
            message = tr("This action will delete the file %1 in both directories. Are you sure you want to perform this action?")
                          .arg(info.name);
            break;
        }
        const QMessageBox::StandardButton button = QMessageBox::question(
            this, tr("Are you sure you want to delete this?"), message);
        if (button != QMessageBox::StandardButton::Yes)
        {
            return;
        }
    }
    const QString leftFile = QDir(leftPath).absoluteFilePath(info.name);
    if (QFile::exists(leftFile))
    {
        if (!QFile::remove(leftFile))
        {
            QMessageBox::critical(
                this, tr("Error while deleting"),
                tr("The file ") + leftFile + tr(" could not be removed."));
            return;
        }
        leftSideChanged = true;

        if (info.result == Compare::Result::LeftSideOnly)
        {
            // Deleting a tree widget item safely removes it from its tree
            // widget, so this is safe to do here.
            delete item;
            item = nullptr;
            return;
        }

        info.result = Compare::Result::RightSideOnly;
        info.leftDate = QDateTime();
        info.leftSize = -1;
        item->setIcon(colIdxResult, QIcon::fromTheme(iconNameRightSideOnly));
        item->setData(colIdxResult, Qt::UserRole, QVariant::fromValue(info));
        item->setText(colIdxLeftDate, tr("none", "no_date"));
        item->setText(colIdxLeftSize, tr("none", "no_size"));
    }

    // Path on right side must exist, because this entry's result was not equal
    // to Compare::Result::LeftSideOnly when we started.
    const QString rightFile = QDir(rightPath).absoluteFilePath(info.name);
    if (!QFile::remove(rightFile))
    {
        QMessageBox::critical(
            this, tr("Error while deleting"),
            tr("The file ") + rightFile + tr(" could not be removed."));
        return;
    }
    rightSideChanged = true;
    delete item;
    item = nullptr;
}

void DirectoryCompareWindow::addLeftSideOnlyEntry(const Compare::Info &info, const QLocale &loc)
{
    QStringList data;
    data.append(info.name);
    data.append(tr("Left side only: ") + info.name);
    data.append(loc.toString(info.leftDate, QLocale::NarrowFormat));
    data.append(tr("none", "no_date"));
    if (info.isDirectory)
    {
        data << tr("Directory") << tr("none", "no_size");
    }
    else
    {
        data.append(loc.formattedDataSize(info.leftSize));
        data.append(tr("none", "no_size"));
    }
    QTreeWidgetItem* item = new QTreeWidgetItem(data);
    item->setIcon(0, info.isDirectory
                         ? icon_provider.icon(QAbstractFileIconProvider::Folder)
                         : icon_provider.icon(QAbstractFileIconProvider::File));
    item->setIcon(colIdxResult, QIcon::fromTheme(iconNameLeftSideOnly));
    item->setData(colIdxResult, Qt::UserRole, QVariant::fromValue(info));
    ui->treeWidget->addTopLevelItem(item);
}

void DirectoryCompareWindow::addRightSideOnlyEntry(const Compare::Info &info, const QLocale &loc)
{
    QStringList data;
    data.append(info.name);
    data.append(tr("Right side only: ") + info.name);
    data.append(tr("none", "no_date"));
    data.append(loc.toString(info.rightDate, QLocale::NarrowFormat));
    if (info.isDirectory)
    {
        data << tr("none", "no_size") << tr("Directory");
    }
    else
    {
        data.append(tr("none", "no_size"));
        data.append(loc.formattedDataSize(info.rightSize));
    }
    QTreeWidgetItem* item = new QTreeWidgetItem(data);
    item->setIcon(0, info.isDirectory
                         ? icon_provider.icon(QAbstractFileIconProvider::Folder)
                         : icon_provider.icon(QAbstractFileIconProvider::File));
    item->setIcon(colIdxResult, QIcon::fromTheme(iconNameRightSideOnly));
    item->setData(colIdxResult, Qt::UserRole, QVariant::fromValue(info));
    ui->treeWidget->addTopLevelItem(item);
}

void DirectoryCompareWindow::addDirectoryExistsEntry(const Compare::Info &info, const QLocale &loc)
{
    QStringList data;
    data.append(info.name);
    data.append(tr("Exists in both directories"));
    data.append(loc.toString(info.leftDate, QLocale::NarrowFormat));
    data.append(loc.toString(info.rightDate, QLocale::NarrowFormat));
    data << tr("Directory") << tr("Directory");

    QTreeWidgetItem* item = new QTreeWidgetItem(data);

    item->setIcon(0, icon_provider.icon(QAbstractFileIconProvider::Folder));

    // Subdirectories are not checked yet, so status is ... questionable / unknown.
    item->setIcon(colIdxResult, QIcon::fromTheme(iconNameUnknown));
    item->setData(colIdxResult, Qt::UserRole, QVariant::fromValue(info));
    ui->treeWidget->addTopLevelItem(item);
}

void DirectoryCompareWindow::addFileEntry(const Compare::Info &info, const QLocale &loc)
{
    QStringList data;
    data.append(info.name);
    switch(info.result)
    {
    case Compare::Result::Identical:
        data.append(tr("Files are identical."));
        break;
    case Compare::Result::Different:
        data.append(tr("Files are different."));
        break;
    case Compare::Result::Unknown:
        data.append(tr("Files could not be compared."));
        break;
    default:
        // Should not happen, but intercept it anyway.
        data.append(tr("Error: Value ") + QString::number(static_cast<int>(info.result)));
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
        icon = QIcon::fromTheme(iconNameIdentical);
        break;
    case Compare::Result::Different:
        icon = QIcon::fromTheme(iconNameDifferent);
        break;
    case Compare::Result::Unknown:
        icon = QIcon::fromTheme(iconNameUnknown);
        break;
    default:
        // Silence compiler warning about unhandled enumeration values
        // LeftSideOnly and RightSideOnly. Those are handled in
        // addLeftSideOnlyEntry() and addRightSideOnlyEntry() before the control
        // flow even gets to this method.
        break;
    }
    if (!icon.isNull())
    {
        item->setIcon(colIdxResult, icon);
    }
    item->setData(colIdxResult, Qt::UserRole, QVariant::fromValue(info));

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
    else if (info.result == Compare::Result::RightSideOnly)
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
