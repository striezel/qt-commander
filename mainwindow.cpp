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

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "fileinfowindow.h"
#include "mountedvolumesdialog.h"

#include <QFileIconProvider>
#include <QMessageBox>

#include "viewers/audioplayerwindow.h"
#include "createdirectorydialog.h"
#include "util/dirutils.h"
#include "util/GitInfos.hpp"
#include "viewers/imageviewwindow.h"
#include "viewers/movieviewwindow.h"
#include "viewers/textviewwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentDirectoryLeft(QDir::home())
    , currentDirectoryRight(QDir::home())
    , settings(Settings())
    , sortActionGroup(QActionGroup(this))
    , whatFirstGroup(QActionGroup(this))
{
    ui->setupUi(this);

    // load any previously saved settings - if any
    settings.load();
    putSettingsIntoGui(settings, true);

    ui->treeWidgetLeft->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    ui->treeWidgetRight->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);

    fillTreeWidget(ui->treeWidgetLeft, QDir::homePath());
    fillTreeWidget(ui->treeWidgetRight, QDir::homePath());

    connect(ui->treeWidgetLeft, &QTreeWidget::itemDoubleClicked,
            this, &MainWindow::treeItemDoubleClicked);
    connect(ui->treeWidgetRight, &QTreeWidget::itemDoubleClicked,
            this, &MainWindow::treeItemDoubleClicked);

    connect(ui->treeWidgetLeft, &QTreeWidget::activated,
            this, &MainWindow::leftTreeWidgetActivated);
    connect(ui->treeWidgetRight, &QTreeWidget::activated,
            this, &MainWindow::rightTreeWidgetActivated);

    connectButtons();
    setUpActionGroups();
    connectMenuActions();

    // focus on left tree view
    ui->treeWidgetLeft->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::textViewerFontChanged(const QFont &new_font)
{
    settings.setTextViewerFont(new_font);
}

void MainWindow::movieViewerAutoStartChanged(const bool autoStart)
{
    settings.setAutoStartVideos(autoStart);
}

void MainWindow::audioPlayerAutoPlayChanged(const bool autoPlay)
{
    settings.setAutoPlayAudio(autoPlay);
}

void MainWindow::audioPlayerVolumeChanged(const int volume)
{
    settings.setAudioVolume(volume);
}

void MainWindow::fillTreeWidget(QTreeWidget* treeWidget, const QString &path, const bool selectFirst)
{
    if ((treeWidget == nullptr) || path.isEmpty())
    {
        return;
    }

    QDir dir(path);
    // Canonicalize path to avoid accumulation of "../" sequences.
    if (path.contains(".."))
    {
        const QString canonical = dir.canonicalPath();
        // Canonical path can be empty due to unresolved symlinks. We don't
        // want that.
        if (!dir.isEmpty())
        {
            dir = QDir(dir.canonicalPath());
        }
    }
    dir.setSorting(settings.getSortFlags());
    dir.setFilter(settings.getFilters());
    const QFileInfoList list = dir.entryInfoList();

    treeWidget->clear();

    QFileIconProvider icon_provider;
    const QIcon directory_icon = icon_provider.icon(QAbstractFileIconProvider::Folder);
    const QIcon file_icon = icon_provider.icon(QAbstractFileIconProvider::File);
    const bool useProvided = settings.getUseProvidedFileIcons();

    const QLocale loc = locale();

    for (const QFileInfo& info: list)
    {
        QStringList data;
        data.append(info.fileName());
        const bool isDirectory = info.isDir();
        data.append(!isDirectory ? QString::number(info.size()) : "Verzeichnis");
        //data.append(info.lastModified().toString(Qt::DateFormat::TextDate));
        data.append(loc.toString(info.lastModified(), QLocale::NarrowFormat));

        QTreeWidgetItem* item = new QTreeWidgetItem(data);
        const QIcon provided_icon = useProvided ? icon_provider.icon(info) : QIcon();
        if (info.isDir())
        {
            item->setIcon(0, provided_icon.isNull() ? directory_icon : provided_icon);
        }
        else
        {
            item->setIcon(0, provided_icon.isNull() ? file_icon : provided_icon);
        }
        item->setTextAlignment(1, Qt::AlignRight);
        treeWidget->addTopLevelItem(item);
    }

    // workaround for empty directory -> gets no entries
    if (list.isEmpty())
    {
        QStringList data;
        data.append("..");
        data.append("Verzeichnis");
        data.append("Unbekannt");
        QTreeWidgetItem* item = new QTreeWidgetItem(data);
        item->setIcon(0, directory_icon);
        item->setTextAlignment(1, Qt::AlignRight);
        treeWidget->addTopLevelItem(item);
    }

    const bool isLeftTree = treeWidget == ui->treeWidgetLeft;
    if (isLeftTree)
    {
      currentDirectoryLeft = dir;
      ui->lnEdPathLeft->setText(currentDirectoryLeft.absolutePath());
    }
    else
    {
        currentDirectoryRight = dir;
        ui->lnEdPathRight->setText(currentDirectoryRight.absolutePath());
    }

    // Adjust width of column for file size.
    treeWidget->resizeColumnToContents(1);
    // Adjust width of column for modification date.
    treeWidget->resizeColumnToContents(2);
    // Column for file name is not adjusted, because that can get way out of
    // hand with longer file names, triggering horizontal scroll bars.

    if (selectFirst)
    {
        treeWidget->setCurrentItem(treeWidget->topLevelItem(0));
    }
}

bool MainWindow::leftTreeIsLatest() const
{
    return ui->treeWidgetLeft->focusTime() > ui->treeWidgetRight->focusTime();
}

QTreeWidget *MainWindow::latestTreeWidget() const
{
    return leftTreeIsLatest() ? ui->treeWidgetLeft : ui->treeWidgetRight;
}

QTreeWidget *MainWindow::otherTreeWidget() const
{
    return leftTreeIsLatest() ? ui->treeWidgetRight : ui->treeWidgetLeft;
}

const QDir &MainWindow::currentDirectory() const
{
    return leftTreeIsLatest() ? currentDirectoryLeft : currentDirectoryRight;
}

const QDir& MainWindow::otherDirectory() const
{
    return leftTreeIsLatest() ? currentDirectoryRight : currentDirectoryLeft;
}

void MainWindow::treeItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (item == nullptr)
        return;

    const bool isLeftTree = item->treeWidget() == ui->treeWidgetLeft;

    const QString new_path = isLeftTree
        ? currentDirectoryLeft.filePath(item->text(0))
        : currentDirectoryRight.filePath(item->text(0));
    const QFileInfo info(new_path);
    if (!info.exists())
    {
        // possibly need to update tree widget here?
        return;
    }
    if (info.isDir())
    {
        const bool selectFirstRow = (column < 0);
        fillTreeWidget(item->treeWidget(), new_path, selectFirstRow);
    }
}

void MainWindow::leftTreeWidgetActivated(const QModelIndex &idx)
{
    QTreeWidgetItem* item = ui->treeWidgetLeft->topLevelItem(idx.row());
    // The column value is not used by treeItemDoubleClicked(), so we use that
    // to pass a negative value which signals to auto-select the first row after
    // the refresh. It's hacky, but there is no way to pass additional
    // parameters to treeItemDoubleClicked(), because it's a slot that has to
    // have a certain method signature.
    treeItemDoubleClicked(item, -1 - idx.column());
}

void MainWindow::rightTreeWidgetActivated(const QModelIndex &idx)
{
    QTreeWidgetItem* item = ui->treeWidgetRight->topLevelItem(idx.row());
    // Note: See the explanation in leftTreeWidgetActivated() for why we pass a
    // negative column value to treeItemDoubleClicked().
    treeItemDoubleClicked(item, -1 - idx.column());
}

void MainWindow::btnRemoveClicked()
{
    QTreeWidget* treeWidget = latestTreeWidget();
    const QList<QTreeWidgetItem*> selection = treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        return;
    }
    QTreeWidgetItem* item = selection.at(0);

    const QString name = item->text(0);

    // Do not operate on parent directory!
    if (name == "..")
    {
        QMessageBox::critical(
            this, "Fehler",
            "Löschoperation kann nicht auf \"..\" ausgeführt werden!");
        return;
    }

    // Ask user whether the file / directory shall really be deleted.
    const int answer = QMessageBox::question(
        this, "Wirklich löschen?", "Soll \"" + name + "\" wirklich gelöscht werden?");
    if (answer != QMessageBox::Yes)
    {
        return;
    }
    QDir& baseDir = treeWidget == ui->treeWidgetLeft
                        ? currentDirectoryLeft
                        : currentDirectoryRight;
    const QFileInfo info(baseDir.absoluteFilePath(name));
    bool success = false;
    if (info.isFile())
    {
        success = baseDir.remove(name);
    }
    else
    {
        success = baseDir.rmdir(name);
    }
    if (!success)
    {
        QString message = "\"" + name + "\" konnte nicht gelöscht werden.";
        if (info.isFile())
        {
            message = "Die Datei " + message;
        }
        else
        {
            message = "Das Verzeichnis " + message
                      + "\n\nMöglicherweise ist das Verzeichnis nicht leer.";
        }
        QMessageBox::critical(this, "Fehler beim Löschen", message);
        return;
    }

    // Delete item from tree view.
    int index = treeWidget->indexOfTopLevelItem(item);
    QTreeWidgetItem* toDelete = treeWidget->takeTopLevelItem(index);
    delete toDelete;
    toDelete = nullptr;

    // If both tree widgets show the same directory, then the other widget needs
    // to be updated, too. That is, we have to remove the corresponding item
    // from the other tree widget, too.
    if (baseDir.absolutePath() == otherDirectory().absolutePath())
    {
        QTreeWidget* other = otherTreeWidget();
        const QList<QTreeWidgetItem*> found = other->findItems(
            name, Qt::MatchCaseSensitive | Qt::MatchFixedString);
        if (found.count() != 1)
        {
            return;
        }
        const int indexToTake = other->indexOfTopLevelItem(found.at(0));
        QTreeWidgetItem* toDeleteOther = other->takeTopLevelItem(indexToTake);
        delete toDeleteOther;
        toDeleteOther = nullptr;
    }
    // ... or when the other tree shows a subdirectory of the deleted directory,
    // then the other tree has to change "upwards" to the directory which still
    // exists.
    else if (DirUtils::isParentOf(baseDir.absoluteFilePath(name), otherDirectory()))
    {
        fillTreeWidget(otherTreeWidget(), baseDir.absolutePath());
    }

    statusBar()->showMessage("'" + name + "' wurde gelöscht.", 5000);
}

void MainWindow::btnCreateDirectoryClicked()
{
    CreateDirectoryDialog* dialog = new CreateDirectoryDialog(this);
    const int choice = dialog->exec();
    if (choice != QDialog::Accepted)
    {
        delete dialog;
        return;
    }

    QString name = dialog->directoryName();
    delete dialog;
    dialog = nullptr;
    if (name.isEmpty() || name.contains("../") || name.contains("..\\"))
    {
        QMessageBox::warning(this, "Ungültiger Verzeichnisname",
                             "Der Verzeichnisname darf nicht leer sein.");
        return;
    }

    QDir& baseDir = leftTreeIsLatest() ? currentDirectoryLeft : currentDirectoryRight;
    const bool success = baseDir.mkdir(name);
    if (!success)
    {
        QMessageBox::critical(
            this, "Fehler beim Erstellen des Verzeichnisses",
            "Das Verzeichnis '" + name + "' konnte nicht erstellt werden.");
        return;
    }

    // refresh corresponding tree widget(s)
    // We might need to refresh both trees, if both tree widgets are showing the
    // same directory.
    if (baseDir.absolutePath() == currentDirectoryLeft.absolutePath())
    {
        fillTreeWidget(ui->treeWidgetLeft, currentDirectoryLeft.absolutePath());
    }
    if (baseDir.absolutePath() == currentDirectoryRight.absolutePath())
    {
        fillTreeWidget(ui->treeWidgetRight, currentDirectoryRight.absolutePath());
    }

    statusBar()->showMessage("Verzeichnis '" + name + "' wurde erstellt.", 5000);
}

void MainWindow::btnMoveClicked()
{
    QTreeWidget* treeWidget = latestTreeWidget();
    const QList<QTreeWidgetItem*> selection = treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        return;
    }
    QTreeWidgetItem* item = selection.at(0);

    const QString name = item->text(0);

    // Do not operate on parent directory!
    if (name == "..")
    {
        QMessageBox::critical(
            this, "Fehler",
            "Verschiebeoperation kann nicht auf \"..\" ausgeführt werden!");
        return;
    }

    if (DirUtils::isSameDir(currentDirectoryLeft, currentDirectoryRight))
    {
        QMessageBox::warning(
            this, "Verschieben in gleiches Verzeichnis nicht möglich",
            "Die beiden Bäume zeigen auf das gleiche Verzeichnis (\""
                + currentDirectoryLeft.absolutePath() + "\"). Ein Verschieben ist daher nicht möglich.");
        return;
    }

    const QString source = currentDirectory().absoluteFilePath(name);
    const QString destination = otherDirectory().absoluteFilePath(name);
    // Move with QFile::rename(). Despite the name QFile it also works for
    // directories, so it's universal.
    if (!QFile::rename(source, destination))
    {
        QMessageBox::critical(
            this, "Fehler beim Verschieben",
            "Das Element '" + name + "' konnte nicht verschoben werden.");
        return;
    }

    // Delete item from current tree view.
    const int index = treeWidget->indexOfTopLevelItem(item);
    QTreeWidgetItem* toDelete = treeWidget->takeTopLevelItem(index);
    delete toDelete;
    toDelete = nullptr;

    // Refresh other tree view.
    fillTreeWidget(otherTreeWidget(), otherDirectory().absolutePath());

    statusBar()->showMessage(
        "'" + name + "' wurde nach " + otherDirectory().absolutePath()
            + " verschoben.", 5000);
}

void MainWindow::btnCopyClicked()
{
    QTreeWidget* treeWidget = latestTreeWidget();
    const QList<QTreeWidgetItem*> selection = treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        return;
    }
    QTreeWidgetItem* item = selection.at(0);

    const QString name = item->text(0);

    // Do not operate on parent directory!
    if (name == "..")
    {
        QMessageBox::critical(
            this, "Fehler",
            "Kopieroperation kann nicht auf \"..\" ausgeführt werden!");
        return;
    }

    if (DirUtils::isSameDir(currentDirectoryLeft, currentDirectoryRight))
    {
        QMessageBox::warning(
            this, "Kopieren in gleiches Verzeichnis nicht möglich",
            "Die beiden Bäume zeigen auf das gleiche Verzeichnis (\""
                + currentDirectoryLeft.absolutePath() + "\"). Ein Kopieren ist daher nicht möglich.");
        return;
    }

    const QString source = currentDirectory().absoluteFilePath(name);
    const QString destination = otherDirectory().absoluteFilePath(name);
    const QFileInfo info(source);
    if (info.isFile())
    {
        // Copy files with QFile::copy().
        if (!QFile::copy(source, destination))
        {
            QMessageBox::critical(
                this, "Fehler beim Kopieren",
                "Die Datei '" + name + "' konnte nicht kopiert werden.");
            return;
        }
    }
    else
    {
        // Directories have to be copied manually and recursively.
        if (!DirUtils::copyRecursively(source, destination))
        {
            QMessageBox::critical(
                this, "Fehler beim Kopieren",
                "Das Verzeichnis '" + name + "' konnte nicht kopiert werden.");
            // Still need to update the other widget, if at least one directory
            // was created.
            const QDir partialCopy(destination);
            if (partialCopy.exists())
            {
                fillTreeWidget(otherTreeWidget(), otherDirectory().absolutePath());
            }
            return;
        }
    }

    // Refresh other tree view.
    fillTreeWidget(otherTreeWidget(), otherDirectory().absolutePath());

    statusBar()->showMessage(
        "'" + name + "' wurde nach " + otherDirectory().absolutePath()
            + " kopiert.", 5000);
}

void MainWindow::btnViewClicked()
{
    QTreeWidget* treeWidget = latestTreeWidget();
    const QList<QTreeWidgetItem*> selection = treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        QMessageBox::information(
            this, "Keine aktive Auswahl vorhanden",
            "Es wurde keine Datei zum Anzeigen ausgewählt.");
        return;
    }
    QTreeWidgetItem* item = selection.at(0);
    const QString name = item->text(0);

    const QString selectedFile = currentDirectory().absoluteFilePath(name);
    const QFileInfo info(selectedFile);

    if (!info.isFile())
    {
        QMessageBox::information(
            this, "Keine Datei ausgewählt",
            QString("Es wurde keine Datei zum Anzeigen ausgewählt.\n\n")
            + "Zum Betrachten können nur Dateien ausgewählt werden, nicht jedoch Verzeichnisse.");
        return;
    }

    const QMimeType mime = detection.getType(info);
    const bool is_supported_image = detection.isSupportedImageFormat(mime);
    const bool is_movie = detection.isMovieFormat(mime);
    const bool is_supported_movie = detection.isSupportedMovieFormat(mime);
    const bool is_audio = detection.isAudioFormat(mime);

    // Inform user of unsupported video format and that we will be using the
    // text viewer for that. Let the user chose whether to continue here.
    if (is_movie && !is_supported_movie)
    {
        const int answer = QMessageBox::warning(
            this, "Format wird nicht unterstützt", "Wiedergabe für das Videoformat '"
                + mime.name() + "' wird nicht unterstützt. Stattdessen wird der"
                + " Textbetrachter verwendet. Soll die Datei als Text angezeigt werden?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (answer != QMessageBox::Yes)
        {
            return;
        }
    }

    if (is_audio)
    {
        // load as audio file
        AudioPlayerWindow* viewer = new AudioPlayerWindow(this);
        if (!viewer->loadAudioFile(selectedFile))
        {
            QMessageBox::critical(
                this, "Fehler beim Öffnen der Datei",
                "Die Datei '" + selectedFile + "' konnte nicht zum Lesen geöffnet werden.");
            delete viewer;
            return;
        }
        viewer->setAutoPlay(settings.getAutoPlayAudio());
        viewer->setVolume(settings.getAudioVolume());
        viewer->setWindowModality(Qt::WindowModality::WindowModal);
        viewer->show();

        // show() returns immediately, so the deletion of viewer is handled by the
        // AudioPlayerWindow itself in its closeEvent();
    }
    else if (is_supported_movie)
    {
        // load as movie
        MovieViewWindow* viewer = new MovieViewWindow(this);
        if (!viewer->loadMovieFile(selectedFile))
        {
            QMessageBox::critical(
                this, "Fehler beim Öffnen der Datei",
                "Die Datei '" + selectedFile + "' konnte nicht zum Lesen geöffnet werden.");
            delete viewer;
            return;
        }
        viewer->setAutoStartVideos(settings.getAutoStartVideos());
        viewer->setWindowModality(Qt::WindowModality::WindowModal);
        viewer->show();

        // show() returns immediately, so the deletion of viewer is handled by the
        // MovieViewWindow itself in its closeEvent();
    }
    else if (is_supported_image)
    {
        // load as image
        ImageViewWindow* viewer = new ImageViewWindow(this);
        if (!viewer->loadImageFile(selectedFile))
        {
            QMessageBox::critical(
                this, "Fehler beim Öffnen der Datei",
                "Die Datei '" + selectedFile + "' konnte nicht zum Lesen geöffnet werden.");
            delete viewer;
            return;
        }
        viewer->setWindowModality(Qt::WindowModality::WindowModal);
        viewer->show();

        // show() returns immediately, so the deletion of viewer is handled by the
        // ImageViewWindow itself in its closeEvent();
    }
    else
    {
        TextViewWindow* viewer = new TextViewWindow(this);
        if (!viewer->loadTextFile(selectedFile))
        {
            QMessageBox::critical(
                this, "Fehler beim Öffnen der Datei",
                "Die Datei '" + selectedFile + "' konnte nicht zum Lesen geöffnet werden.");
            delete viewer;
            return;
        }
        viewer->setFont(settings.getTextViewerFont());
        viewer->setWindowModality(Qt::WindowModality::WindowModal);
        viewer->show();

        // show() returns immediately, so the deletion of viewer is handled by the
        // TextViewWindow itself in its closeEvent();
    }
}

void MainWindow::actionShowFileInfoTriggered()
{
    QTreeWidget* treeWidget = latestTreeWidget();
    const QList<QTreeWidgetItem*> selection = treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        QMessageBox::information(
            this, "Keine aktive Auswahl vorhanden",
            "Es wurde weder eine Datei noch ein Verzeichnis ausgewählt, dessen Eigenschaften angezeigt werden könnten.");
        return;
    }
    QTreeWidgetItem* item = selection.at(0);
    const QString name = item->text(0);

    const QString selectedFile = currentDirectory().absoluteFilePath(name);

    FileInfoWindow* window = new FileInfoWindow(this);
    window->loadInformation(selectedFile);
    window->setWindowModality(Qt::WindowModality::WindowModal);
    window->show();

    // show() returns immediately, so the deletion of viewer is handled by the
    // FileInfoWindow itself in its closeEvent();
}

void MainWindow::refreshCurrentView()
{
    refreshView(latestTreeWidget(), currentDirectory(), true);
}

void MainWindow::refreshBothViews()
{
    refreshView(ui->treeWidgetLeft, currentDirectoryLeft, false);
    refreshView(ui->treeWidgetRight, currentDirectoryRight, false);
}

void MainWindow::refreshView(QTreeWidget *treeWidget, const QDir &dir, const bool showStatusMessage)
{
    if (treeWidget == nullptr)
    {
        return;
    }

    const QString path = dir.absolutePath();
    fillTreeWidget(treeWidget, path);

    if (showStatusMessage)
    {
        statusBar()->showMessage("Ansicht für Verzeichnis '" + path + "' ("
                                     + (treeWidget == ui->treeWidgetLeft ? "links" : "rechts")
                                     + ") wurde aktualisiert.", 5000);
    }
}

void MainWindow::connectButtons()
{
    connect(ui->btnView, &QPushButton::clicked, this, &MainWindow::btnViewClicked);
    connect(ui->btnCopy, &QPushButton::clicked, this, &MainWindow::btnCopyClicked);
    connect(ui->btnMove, &QPushButton::clicked, this, &MainWindow::btnMoveClicked);
    connect(ui->btnCreateDirectory, &QPushButton::clicked, this, &MainWindow::btnCreateDirectoryClicked);
    connect(ui->btnRemove, &QPushButton::clicked, this, &MainWindow::btnRemoveClicked);
    connect(ui->btnExit, &QPushButton::clicked, this, &MainWindow::close);
}

void MainWindow::setUpActionGroups()
{
    sortActionGroup.addAction(ui->actionSortByName);
    sortActionGroup.addAction(ui->actionSortByTime);
    sortActionGroup.addAction(ui->actionSortBySize);
    sortActionGroup.addAction(ui->actionSortByType);

    whatFirstGroup.setExclusionPolicy(QActionGroup::ExclusionPolicy::ExclusiveOptional);
    whatFirstGroup.addAction(ui->actionSortFilesFirst);
    whatFirstGroup.addAction(ui->actionSortDirectoriesFirst);
}

void MainWindow::putSettingsIntoGui(const Settings& settings, const bool avoidRefresh)
{
    const QDir::Filters new_filters = settings.getFilters();
    const QDir::SortFlags new_sort_flags = settings.getSortFlags();

    // update checked property for all relevant menu items/actions
    ui->actionShowHiddenFiles->setChecked(new_filters.testFlag(QDir::Filter::Hidden));
    ui->actionShowSystemFiles->setChecked(new_filters.testFlag(QDir::Filter::System));
    ui->actionHideFiles->setChecked(!new_filters.testFlag(QDir::Filter::Files));

    ui->actionSortByName->setChecked(!new_sort_flags.testAnyFlags(
        QDir::SortFlag::Time | QDir::SortFlag::Size | QDir::SortFlag::Type));
    ui->actionSortByTime->setChecked(new_sort_flags.testFlag(QDir::SortFlag::Time));
    ui->actionSortBySize->setChecked(new_sort_flags.testFlag(QDir::SortFlag::Size));
    ui->actionSortByType->setChecked(new_sort_flags.testFlag(QDir::SortFlag::Type));

    ui->actionReverseSort->setChecked(new_sort_flags.testFlag(QDir::SortFlag::Reversed));

    ui->actionSortIgnoreCase->setChecked(new_sort_flags.testFlag(QDir::SortFlag::IgnoreCase));

    ui->actionSortFilesFirst->setChecked(new_sort_flags.testFlag(QDir::SortFlag::DirsLast));
    ui->actionSortDirectoriesFirst->setChecked(new_sort_flags.testFlag(QDir::SortFlag::DirsFirst));

    ui->actionUseProvidedIcons->setChecked(settings.getUseProvidedFileIcons());

    // Check whether tree widgets need a refresh.
    const bool needs_refresh = (new_filters != this->settings.getFilters())
                               || (new_sort_flags != this->settings.getSortFlags()
                               || (settings.getUseProvidedFileIcons() != this->settings.getUseProvidedFileIcons()));

    // New values for filters and sort flags need to be set before a potential
    // refresh happens, because the refresh uses the currently set flags.
    this->settings = settings;

    if (needs_refresh && !avoidRefresh)
    {
        refreshBothViews();
    }
}

void MainWindow::connectMenuActions()
{
    connect(ui->actionShowFileInfo, &QAction::triggered, this, &MainWindow::actionShowFileInfoTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    connect(ui->actionShowHiddenFiles, &QAction::triggered, this, &MainWindow::actionShowHiddenFilesTriggered);
    connect(ui->actionShowSystemFiles, &QAction::triggered, this, &MainWindow::actionShowSystemFilesTriggered);
    connect(ui->actionHideFiles, &QAction::triggered, this, &MainWindow::actionHideFilesTriggered);

    connect(ui->actionSortByName, &QAction::triggered, this, &MainWindow::actionSortBySomethingTriggered);
    connect(ui->actionSortByTime, &QAction::triggered, this, &MainWindow::actionSortBySomethingTriggered);
    connect(ui->actionSortBySize, &QAction::triggered, this, &MainWindow::actionSortBySomethingTriggered);
    connect(ui->actionSortByType, &QAction::triggered, this, &MainWindow::actionSortBySomethingTriggered);

    connect(ui->actionReverseSort, &QAction::triggered, this, &MainWindow::actionReverseSortTriggered);

    connect(ui->actionSortIgnoreCase, &QAction::triggered, this, &MainWindow::actionSortIgnoreCaseTriggered);

    connect(ui->actionSortFilesFirst, &QAction::triggered, this, &MainWindow::actionSortSomethingFirstTriggered);
    connect(ui->actionSortDirectoriesFirst, &QAction::triggered, this, &MainWindow::actionSortSomethingFirstTriggered);

    connect(ui->actionUseProvidedIcons, &QAction::triggered, this, &MainWindow::actionUseProvidedIconsTriggered);

    connect(ui->actionRefresh, &QAction::triggered, this, &MainWindow::actionRefreshTriggered);

    connect(ui->actionSaveSettings, &QAction::triggered, this, &MainWindow::actionSaveSettingsTriggered);
    connect(ui->actionLoadSettings, &QAction::triggered, this, &MainWindow::actionLoadSettingsTriggered);
    connect(ui->actionRestoreDefaultSettings, &QAction::triggered, this, &MainWindow::actionRestoreDefaultSettingsTriggered);

    connect(ui->actionShowMountpoints, &QAction::triggered, this, &MainWindow::actionShowMountpointsTriggered);
    connect(ui->actionAboutQtCommander, &QAction::triggered, this, &MainWindow::actionAboutQtCmdrTriggered);
    connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindow::actionAboutQtTriggered);
}

void MainWindow::actionRefreshTriggered()
{
    refreshCurrentView();
}

void MainWindow::actionSaveSettingsTriggered()
{
    settings.save();
}

void MainWindow::actionLoadSettingsTriggered()
{
    Settings loaded_settings;
    loaded_settings.load();

    putSettingsIntoGui(loaded_settings);
}

void MainWindow::actionRestoreDefaultSettingsTriggered()
{
    Settings restored_settings;
    restored_settings.resetToDefaults();

    putSettingsIntoGui(restored_settings);
}

void MainWindow::actionShowMountpointsTriggered()
{
    MountedVolumesDialog dialog;
    dialog.exec();
}

void MainWindow::actionAboutQtCmdrTriggered()
{
    qtcmdr::GitInfos info;
    QString message = QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion()
                      + "\n\nVersion control commit: " + QString::fromStdString(info.commit().substr(0, 7))
                      + "\nVersion control date: " + QString::fromStdString(info.date());
    QMessageBox::about(this, QCoreApplication::applicationName(), message);
}

void MainWindow::actionAboutQtTriggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::actionShowHiddenFilesTriggered(bool checked)
{
    qDebug() << "Old filter setting: " << settings.getFilters();
    if (checked)
    {
        settings.setFilters(settings.getFilters() | QDir::Filter::Hidden);
    }
    else
    {
        settings.setFilters(settings.getFilters() ^ QDir::Filter::Hidden);
    }
    qDebug() << "New filter setting: " << settings.getFilters();
    refreshBothViews();
}

void MainWindow::actionShowSystemFilesTriggered(bool checked)
{
    qDebug() << "Old filter setting: " << settings.getFilters();
    if (checked)
    {
        settings.setFilters(settings.getFilters() | QDir::Filter::System);
    }
    else
    {
        settings.setFilters(settings.getFilters() ^ QDir::Filter::System);
    }
    qDebug() << "New filter setting: " << settings.getFilters();
    refreshBothViews();
}

void MainWindow::actionHideFilesTriggered(bool checked)
{
    qDebug() << "Old filter setting: " << settings.getFilters();
    if (checked)
    {
        settings.setFilters(settings.getFilters() ^ QDir::Filter::Files);
    }
    else
    {
        settings.setFilters(settings.getFilters() | QDir::Filter::Files);
    }
    qDebug() << "New filter setting: " << settings.getFilters();
    refreshBothViews();
}

void MainWindow::actionSortBySomethingTriggered(bool checked)
{
    if (!checked)
    {
        return;
    }

    qDebug() << "Old sort setting: " << settings.getSortFlags();

    QDir::SortFlag sortByFlag = QDir::SortFlag::Name;
    if (sender() == ui->actionSortByName)
        sortByFlag = QDir::SortFlag::Name;
    else if (sender() == ui->actionSortByTime)
        sortByFlag = QDir::SortFlag::Time;
    else if (sender() == ui->actionSortBySize)
        sortByFlag = QDir::SortFlag::Size;
    else // must be sort by type then
        sortByFlag = QDir::SortFlag::Type;

    // clear other "sort by ..." flags, because only one of them can be used
    // Note: QDir::SortFlag::Name is zero, so it needs not to be tested.
    const auto sortFlags = settings.getSortFlags();
    if (sortFlags.testFlag(QDir::SortFlag::Time))
        settings.setSortFlags(settings.getSortFlags() ^ QDir::SortFlag::Time);
    if (sortFlags.testFlag(QDir::SortFlag::Size))
        settings.setSortFlags(settings.getSortFlags() ^ QDir::SortFlag::Size);
    if (sortFlags.testFlag(QDir::SortFlag::Type))
        settings.setSortFlags(settings.getSortFlags() ^ QDir::SortFlag::Type);

    // Set new sort flag.
    settings.setSortFlags(settings.getSortFlags() | sortByFlag);
    qDebug() << "New sort setting: " << settings.getSortFlags();
    refreshBothViews();
}

void MainWindow::actionReverseSortTriggered(bool checked)
{
    qDebug() << "Old sort setting: " << settings.getSortFlags();
    if (checked)
    {
        settings.setSortFlags(settings.getSortFlags() | QDir::SortFlag::Reversed);
    }
    else
    {
        settings.setSortFlags(settings.getSortFlags() ^ QDir::SortFlag::Reversed);
    }
    qDebug() << "New sort setting: " << settings.getSortFlags();
    refreshBothViews();
}

void MainWindow::actionSortIgnoreCaseTriggered(bool checked)
{
    qDebug() << "Old sort setting: " << settings.getSortFlags();
    if (checked)
    {
        settings.setSortFlags(settings.getSortFlags() | QDir::SortFlag::IgnoreCase);
    }
    else
    {
        settings.setSortFlags(settings.getSortFlags() ^ QDir::SortFlag::IgnoreCase);
    }
    qDebug() << "New sort setting: " << settings.getSortFlags();
    refreshBothViews();
}

void MainWindow::actionSortSomethingFirstTriggered(bool checked)
{
    qDebug() << "Old sort setting: " << settings.getSortFlags();
    QDir::SortFlag sortFirstFlag = QDir::SortFlag::DirsFirst;
    if (sender() == ui->actionSortFilesFirst)
    {
        sortFirstFlag = QDir::SortFlag::DirsLast;
    }

    if (checked)
    {
        // clear other flag
        const QDir::SortFlag otherFlag = sortFirstFlag == QDir::SortFlag::DirsFirst
                                             ? QDir::SortFlag::DirsLast
                                             : QDir::SortFlag::DirsFirst;
        if (settings.getSortFlags().testFlag(otherFlag))
        {
            settings.setSortFlags(settings.getSortFlags() ^ otherFlag);
        }
        // set flag
        settings.setSortFlags(settings.getSortFlags() | sortFirstFlag);
    }
    else
    {
        // clear flag
        if (settings.getSortFlags().testFlag(sortFirstFlag))
        {
            settings.setSortFlags(settings.getSortFlags() ^ sortFirstFlag);
        }
    }
    qDebug() << "New sort setting: " << settings.getSortFlags();
    refreshBothViews();
}

void MainWindow::actionUseProvidedIconsTriggered(bool checked)
{
    settings.setUseProvidedFileIcons(checked);
    refreshBothViews();
}
