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

#include <QDir>
#include <QMessageBox>
#include <QStringList>
#include <QTreeWidgetItem>

DirectoryCompareWindow::DirectoryCompareWindow(const QString& pathLeft, const QString& pathRight, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DirectoryCompareWindow)
    , initialShowDone(false)
    , leftPath(pathLeft)
    , rightPath(pathRight)
    , compare(nullptr)
{
    ui->setupUi(this);

    ui->lineEditLeft->setText(leftPath);
    ui->lineEditRight->setText(rightPath);

    connect(ui->actionClose, &QAction::triggered, this, &DirectoryCompareWindow::threadHandlingClose);
    connect(ui->btnCancel, &QPushButton::clicked, this, &DirectoryCompareWindow::btnCancelClicked);

    connect(ui->actionCopyToLeft, &QAction::triggered, this, &DirectoryCompareWindow::actionCopyToLeftTriggered);
    connect(ui->actionCopyToRight, &QAction::triggered, this, &DirectoryCompareWindow::actionCopyToRightTriggered);
    connect(ui->treeWidget, &QTreeWidget::itemSelectionChanged, this, &DirectoryCompareWindow::treeWidgetSelectionChanged);
}

DirectoryCompareWindow::~DirectoryCompareWindow()
{
    delete ui;
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

    compareDirectories(leftPath, rightPath);
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
    statusBar()->showMessage("Vergleich ist abgeschlossen.");
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
    ui->treeWidget->resizeColumnToContents(2);
    ui->treeWidget->resizeColumnToContents(3);
    // Adjust width of columns with file sizes.
    ui->treeWidget->resizeColumnToContents(4);
    ui->treeWidget->resizeColumnToContents(5);

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);

    ui->btnCancel->setEnabled(false);
}

void DirectoryCompareWindow::compareCancelled(const QList<Compare::Info> &list)
{
    statusBar()->showMessage("Vergleich wurde abgebrochen.");
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
        return;
    }

    const QTreeWidgetItem* item = selection.at(0);
    const Compare::Result selectedResult =
        item->data(1, Qt::UserRole).value<Compare::Info>().result;
    ui->actionCopyToLeft->setEnabled(selectedResult == Compare::Result::RightSideOnly);
    ui->actionCopyToRight->setEnabled(selectedResult == Compare::Result::LeftSideOnly);
}

void DirectoryCompareWindow::actionCopyToLeftTriggered()
{
    const QList<QTreeWidgetItem*> selection = ui->treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        QMessageBox::warning(this, "Keine Auswahl vorhanden",
                             "Es wurde keine Datei zum Kopieren ausgewählt.");
        return;
    }
    QTreeWidgetItem* item = selection.at(0);
    Compare::Info info = item->data(1, Qt::UserRole).value<Compare::Info>();
    if (info.result == Compare::Result::Identical)
    {
        QMessageBox::warning(
            this, "Kopieren nicht notwendig",
            "Die Datei ist in beiden Verzeichnissen identisch. Ein Kopieren ist daher nicht notwendig.");
        return;
    }
    if (info.result == Compare::Result::LeftSideOnly)
    {
        QMessageBox::warning(
            this, "Kopieren nicht möglich",
            QStringLiteral("Die Datei ist nur auf der linken Seite vorhanden. ")
                + "Ein Kopieren von rechts nach links ist daher nicht möglich.");
        return;
    }
    if (info.isDirectory)
    {
        QMessageBox::warning(
            this, "Kopieren von Verzeichnissen",
            QStringLiteral("Der ausgewählte Eintrag ist ein Verzeichnis. ")
                + "Ein Kopieren von Verzeichnissen ist in dieser Ansicht aber nicht möglich.");
        return;
    }

    const QString name = info.name;
    const QString source = QDir(rightPath).absoluteFilePath(name);
    const QString destination = QDir(leftPath).absoluteFilePath(name);

    if (!QFile::copy(source, destination))
    {
        QMessageBox::critical(
            this, "Kopieren fehlgeschlagen",
            QStringLiteral("Die Datei ") + name + " konnte nicht nach "
                + destination + " kopiert werden");
        return;
    }

    // Adjust widget item.
    item->setIcon(1, QIcon::fromTheme("document-new"));
    item->setText(1, "Dateien sind identisch.");
    info.result = Compare::Result::Identical;
    const QFileInfo fileInfo = QFileInfo(destination);
    info.leftDate = fileInfo.lastModified();
    info.leftSize = fileInfo.size();
    const QLocale loc = locale();
    item->setText(2, loc.toString(info.leftDate, QLocale::NarrowFormat));
    item->setText(4, loc.formattedDataSize(info.leftSize));
    item->setData(1, Qt::UserRole, QVariant::fromValue(info));
}

void DirectoryCompareWindow::actionCopyToRightTriggered()
{
    const QList<QTreeWidgetItem*> selection = ui->treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        QMessageBox::warning(this, "Keine Auswahl vorhanden",
                             "Es wurde keine Datei zum Kopieren ausgewählt.");
        return;
    }
    QTreeWidgetItem* item = selection.at(0);
    Compare::Info info = item->data(1, Qt::UserRole).value<Compare::Info>();
    if (info.result == Compare::Result::Identical)
    {
        QMessageBox::warning(
            this, "Kopieren nicht notwendig",
            "Die Datei ist in beiden Verzeichnissen identisch. Ein Kopieren ist daher nicht notwendig.");
        return;
    }
    if (info.result == Compare::Result::RightSideOnly)
    {
        QMessageBox::warning(
            this, "Kopieren nicht möglich",
            QStringLiteral("Die Datei ist nur auf der rechten Seite vorhanden. ")
                + "Ein Kopieren von links nach rechts ist daher nicht möglich.");
        return;
    }
    if (info.isDirectory)
    {
        QMessageBox::warning(
            this, "Kopieren von Verzeichnissen",
            QStringLiteral("Der ausgewählte Eintrag ist ein Verzeichnis. ")
                + "Ein Kopieren von Verzeichnissen ist in dieser Ansicht aber nicht möglich.");
        return;
    }

    const QString name = info.name;
    const QString source = QDir(leftPath).absoluteFilePath(name);
    const QString destination = QDir(rightPath).absoluteFilePath(name);

    if (!QFile::copy(source, destination))
    {
        QMessageBox::critical(
            this, "Kopieren fehlgeschlagen",
            QStringLiteral("Die Datei ") + name + " konnte nicht nach "
                + destination + " kopiert werden");
        return;
    }

    // Adjust widget item.
    item->setIcon(1, QIcon::fromTheme("document-new"));
    item->setText(1, "Dateien sind identisch.");
    info.result = Compare::Result::Identical;
    const QFileInfo fileInfo = QFileInfo(destination);
    info.rightDate = fileInfo.lastModified();
    info.rightSize = fileInfo.size();
    const QLocale loc = locale();
    item->setText(3, loc.toString(info.rightDate, QLocale::NarrowFormat));
    item->setText(5, loc.formattedDataSize(info.rightSize));
    item->setData(1, Qt::UserRole, QVariant::fromValue(info));
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
    item->setIcon(0, info.isDirectory
                         ? icon_provider.icon(QAbstractFileIconProvider::Folder)
                         : icon_provider.icon(QAbstractFileIconProvider::File));
    item->setIcon(1, QIcon::fromTheme("go-previous"));
    item->setData(1, Qt::UserRole, QVariant::fromValue(info));
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
    item->setIcon(0, info.isDirectory
                         ? icon_provider.icon(QAbstractFileIconProvider::Folder)
                         : icon_provider.icon(QAbstractFileIconProvider::File));
    item->setIcon(1, QIcon::fromTheme("go-next"));
    item->setData(1, Qt::UserRole, QVariant::fromValue(info));
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

    item->setIcon(0, icon_provider.icon(QAbstractFileIconProvider::Folder));

    // Subdirectories are not checked yet, so status is ... questionable / unknown.
    item->setIcon(1, QIcon::fromTheme("dialog-question"));
    item->setData(1, Qt::UserRole, QVariant::fromValue(info));
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
    default:
        // Silence compiler warning about unhandled enumeration values
        // LeftSideOnly and RightSideOnly. Those are handled in
        // addLeftSideOnlyEntry() and addRightSideOnlyEntry() before the control
        // flow even gets to this method.
        break;
    }
    if (!icon.isNull())
    {
        item->setIcon(1, icon);
    }
    item->setData(1, Qt::UserRole, QVariant::fromValue(info));

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
