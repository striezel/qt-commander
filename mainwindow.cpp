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
#include "directorycomparewindow.h"
#include "settingsdialog.h"

#include <QFileIconProvider>
#include <QMessageBox>

#include "viewers/audioplayerwindow.h"
#include "checksumdialog.h"
#include "createdirectorydialog.h"
#include "util/dirutils.h"
#include "util/GitInfos.hpp"
#include "viewers/imageviewwindow.h"
#include "viewers/animationviewwindow.h"
#include "viewers/pdfviewwindow.h"
#include "viewers/textviewwindow.h"
#include "viewers/videoplayerwindow.h"

const QString MainWindow::translatorPrefix{QStringLiteral("qt-commander_")};
const QString MainWindow::translatorDirectory{
#if defined(__linux__) && !defined(USE_EMPTY_TRANSLATION_DIRECTORY)
    QStringLiteral("/usr/share/qt-commander/translations")
#else
    QStringLiteral("")
#endif
};

MainWindow::MainWindow(QWidget *parent, const QStringList& positionalArgs)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentDirectoryLeft(QDir::home())
    , currentDirectoryRight(QDir::home())
    , settings(Settings())
    , sortActionGroup(QActionGroup(this))
    , whatFirstGroup(QActionGroup(this))
    , languageGroup(QActionGroup(this))
{
    ui->setupUi(this);

    // load previously saved settings - if any
    settings.load();
    putSettingsIntoGui(settings, true);

    attemptToLoadMatchingTranslation();

    ui->treeWidgetLeft->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
    ui->treeWidgetRight->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);

    fillTreeWidget(ui->treeWidgetLeft, initialDirectory(positionalArgs, 0));
    fillTreeWidget(ui->treeWidgetRight, initialDirectory(positionalArgs, 1));

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

QString MainWindow::initialDirectory(const QStringList &positionalArgs, const qsizetype index) const
{
    if (positionalArgs.size() > index)
    {
        const QFileInfo info (positionalArgs.at(index));
        if (info.isDir())
        {
            // Use given directory as initial directory.
            return info.absoluteFilePath();
        }
        else if (info.exists())
        {
            // Use parent directory of file as initial directory.
            return info.absolutePath();
        }
    }

    // Default: Use the home directory.
    return QDir::homePath();
}

void MainWindow::textViewerFontChanged(const QFont &new_font)
{
    settings.setTextViewerFont(new_font);
}

void MainWindow::textViewerAutoSelectChanged(const bool autoSelect)
{
    settings.setTextViewerAutoSelectLanguage(autoSelect);
}

void MainWindow::textViewerHightlightingThemeChanged(const ThemeId theme)
{
    settings.setTextViewerHighlightingTheme(theme);
}

void MainWindow::videoPlayerAutoPlayChanged(const bool autoPlay)
{
    settings.setAutoPlayVideo(autoPlay);
}

void MainWindow::videoPlayerLoopForeverChanged(const bool loopForever)
{
    settings.setLoopVideoForever(loopForever);
}

void MainWindow::videoPlayerVolumeChanged(const int volume)
{
    settings.setVideoVolume(volume);
}

void MainWindow::audioPlayerAutoPlayChanged(const bool autoPlay)
{
    settings.setAutoPlayAudio(autoPlay);
}

void MainWindow::audioPlayerLoopForeverChanged(const bool loopForever)
{
    settings.setLoopAudioForever(loopForever);
}

void MainWindow::audioPlayerVolumeChanged(const int volume)
{
    settings.setAudioVolume(volume);
}

void MainWindow::selectedHashAlgorithmChanged(const QCryptographicHash::Algorithm algorithm)
{
    settings.setSelectedHashAlgorithm(algorithm);
}

void MainWindow::changeLeftTree(const QString& newPath)
{
    QDir dir(newPath);
    if (!dir.exists())
    {
        return;
    }

    fillTreeWidget(ui->treeWidgetLeft, dir.absolutePath());
}

void MainWindow::changeRightTree(const QString& newPath)
{
    QDir dir(newPath);
    if (!dir.exists())
    {
        return;
    }

    fillTreeWidget(ui->treeWidgetRight, dir.absolutePath());
}

void MainWindow::fillTreeWidget(QTreeWidget* treeWidget, const QString &path, const bool selectFirst, const QString& selectedItemName)
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
    const bool showFormatted = settings.getShowFormattedSize();

    const QLocale loc = locale();

    for (const QFileInfo& info: list)
    {
        QStringList data;
        data.append(info.fileName());
        const bool isDirectory = info.isDir();
        data.append(!isDirectory
                        ? (showFormatted ? loc.formattedDataSize(info.size()) : QString::number(info.size()))
                        : tr("Directory"));
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
        data.append(tr("Directory"));
        data.append(tr("Unknown"));
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
    if (!selectedItemName.isEmpty())
    {
        const QList<QTreeWidgetItem*> candidates = treeWidget->findItems(
            selectedItemName, Qt::MatchFlag::MatchFixedString | Qt::MatchFlag::MatchCaseSensitive);
        if (!candidates.isEmpty())
        {
            treeWidget->setCurrentItem(candidates.at(0));
            treeWidget->scrollToItem(candidates.at(0), QTreeWidget::ScrollHint::PositionAtCenter);
        }
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
    {
        return;
    }

    const bool isLeftTree = item->treeWidget() == ui->treeWidgetLeft;

    const QString new_path = QDir::cleanPath(
        isLeftTree
            ? currentDirectoryLeft.filePath(item->text(0))
            : currentDirectoryRight.filePath(item->text(0)));
    const QFileInfo info(new_path);
    if (!info.exists())
    {
        // possibly need to update tree widget here?
        return;
    }
    if (info.isDir())
    {
        QString selectedItemName;
        // If we are moving to the parent directory, then set the directory we
        // came from as selected item.
        if (item->text(0) == "..")
        {
            const QString current_directory = (isLeftTree ? currentDirectoryLeft : currentDirectoryRight).path();
            selectedItemName = QFileInfo(current_directory).fileName();
        }
        const bool selectFirstRow = (column < 0);
        fillTreeWidget(item->treeWidget(), new_path, selectFirstRow, selectedItemName);
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

void MainWindow::btnGoUpClicked()
{
    const bool leftSide = sender() == ui->btnGoUpLeft;
    const QDir& directory = leftSide ? currentDirectoryLeft : currentDirectoryRight;
    if (directory.isRoot())
    {
        QMessageBox::information(this, tr("Already in root directory"),
                                 tr("You already are in the root directory. There is no parent directory for that directory."));
        return;
    }
    const QFileInfo info = QFileInfo(directory.path());
    QTreeWidget * treeWidget = leftSide ? ui->treeWidgetLeft : ui->treeWidgetRight;
    fillTreeWidget(treeWidget, info.path(), false, info.fileName());
}

void MainWindow::btnGoHomeClicked()
{
    const QString path = QDir::homePath();
    if (sender() == ui->btnGoHomeLeft)
    {
        fillTreeWidget(ui->treeWidgetLeft, path, true);
    }
    else
    {
        fillTreeWidget(ui->treeWidgetRight, path, true);
    }
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
            this, tr("Error"),
            tr("The delete operation cannot be performed on \"..\"!"));
        return;
    }

    // Ask user whether the file / directory shall really be deleted.

    const QString title = settings.getDeleteMovesToTrash()
                              ? tr("Move to the recycle bin?")
                              : tr("Do you really want to delete this?");
    const QString text = settings.getDeleteMovesToTrash()
                             ?  tr("Do you really want to move %1 to the recycle bin?").arg(name)
                             :  tr("Do you really want to delete %1?").arg(name);
    const int answer = QMessageBox::question(this, title, text);
    if (answer != QMessageBox::Yes)
    {
        return;
    }
    QDir& baseDir = treeWidget == ui->treeWidgetLeft
                        ? currentDirectoryLeft
                        : currentDirectoryRight;
    const QFileInfo info(baseDir.absoluteFilePath(name));
    bool success = false;
    if (!settings.getDeleteMovesToTrash())
    {
        if (info.isDir())
        {
            if (settings.getRecursiveDeleteEnabled())
            {
                QDir dirToRemove(baseDir.absoluteFilePath(name));
                // Note: This may take long for large recursive directory trees.
                // TODO: Implement deletion with progress meter and possibility
                // to cancel the deletion when it takes too long.
                success = dirToRemove.removeRecursively();
            }
            else
            {
                // Try remove the directory. Only works when directory is empty.
                success = baseDir.rmdir(name);
            }
        }
        else
        {
            success = baseDir.remove(name);
        }
        if (!success)
        {
            QString message = "\"" + name + tr("\" could not be deleted.");
            if (info.isFile())
            {
                message = tr("The file ") + message;
            }
            else
            {
                message = tr("The directory ") + message
                          + "\n\n" + tr("Maybe the directory is not empty.");
            }
            QMessageBox::critical(this, tr("Error while deleting"), message);
            return;
        }
    }
    else
    {
        success = QFile::moveToTrash(info.absoluteFilePath());
        if (!success)
        {
            QString message = "\"" + name + tr("\" could not be moved into the recycle bin.");
            if (info.isFile())
            {
                message = tr("The file ") + message;
            }
            else
            {
                message = tr("The directory ") + message;
            }
            QMessageBox::critical(this, tr("Error while moving to recycle bin"), message);
            return;
        }
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

    if (settings.getDeleteMovesToTrash())
    {
        statusBar()->showMessage(tr("'%1' has been moved into the recycle bin.").arg(name), 5000);
    }
    else
    {
        statusBar()->showMessage(tr("'%1' has been deleted.").arg(name), 5000);
    }
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
        QMessageBox::warning(this, tr("Invalid directory name"),
                             tr("The directory name must not be empty."));
        return;
    }

    QDir& baseDir = leftTreeIsLatest() ? currentDirectoryLeft : currentDirectoryRight;
    const bool success = baseDir.mkdir(name);
    if (!success)
    {
        QMessageBox::critical(
            this, tr("Error while creating the directory"),
            tr("The directory '") + name + tr("' could not be created."));
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

    statusBar()->showMessage(tr("Directory '") + name + tr("' has been created."), 5000);
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
            this, tr("Error"),
            tr("Move operation cannot be performed on \"..\"!"));
        return;
    }

    if (DirUtils::isSameDir(currentDirectoryLeft, currentDirectoryRight))
    {
        QMessageBox::warning(
            this, tr("Moving to same directory is not possible"),
            tr("Both views show the same directory (\"%1\"). Therefore, a move is not possible.")
                .arg(currentDirectoryLeft.absolutePath()));
        return;
    }

    const QString source = currentDirectory().absoluteFilePath(name);
    const QString destination = otherDirectory().absoluteFilePath(name);
    // Move with QFile::rename(). Despite the name QFile it also works for
    // directories, so it's universal.
    if (!QFile::rename(source, destination))
    {
        QMessageBox::critical(
            this, tr("Error while moving"),
            tr("The element '%1' could not be moved.").arg(name));
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
        tr("'%1' has been moved to %2.").arg(name)
            .arg(otherDirectory().absolutePath()), 5000);
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
            this, tr("Error"),
            tr("Copy operation cannot be performed on \"..\"!"));
        return;
    }

    if (DirUtils::isSameDir(currentDirectoryLeft, currentDirectoryRight))
    {
        QMessageBox::warning(
            this, tr("Copying to same directory is not possible"),
            tr("Both views show the same directory (\"%1\"). Therefore, a copy is not possible.")
                .arg(currentDirectoryLeft.absolutePath()));
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
                this, tr("Error while copying"),
                tr("The file '%1' could not be copied.").arg(name));
            return;
        }
    }
    else
    {
        // Directories have to be copied manually and recursively.
        if (!DirUtils::copyRecursively(source, destination))
        {
            QMessageBox::critical(
                this, tr("Error while copying"),
                tr("The directory '%1' could not be copied.").arg(name));
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
        tr("'%1' was copied to %2.").arg(name)
            .arg(otherDirectory().absolutePath()), 5000);
}

void MainWindow::btnViewClicked()
{
    QTreeWidget* treeWidget = latestTreeWidget();
    const QList<QTreeWidgetItem*> selection = treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        QMessageBox::information(
            this, tr("No file selected"),
            tr("No file has been selected to be viewed."));
        return;
    }
    QTreeWidgetItem* item = selection.at(0);
    const QString name = item->text(0);

    const QString selectedFile = currentDirectory().absoluteFilePath(name);
    const QFileInfo info(selectedFile);

    if (info.isDir())
    {
        // Change to directory.
        treeItemDoubleClicked(item, -1);
        return;
    }
    if (!info.isFile())
    {
#if defined(__linux__)
        const QString title = tr("No suitable file selected");
        const QString message =
            tr("No suitable file has been selected to be displayed.") + "\n\n"
            + tr("Only files can be selected for viewing. Directories cannot be viewed.")
            + tr(" Special file types like block devices, FIFOs or sockets cannot be viewed either.");
#else
        const QString title = tr("No file selected");
        const QString message = tr("No file has been selected to be displayed.") + "\n\n"
            + tr("Only files can be selected for viewing. Directories cannot be viewed.");
#endif
        QMessageBox::information(this, title, message);
        return;
    }

    const QMimeType mime = detection.getType(info);
    const bool is_supported_image = detection.isSupportedImageFormat(mime);
    const bool is_supported_movie = detection.isSupportedMovieFormat(mime);
    const bool is_audio = FileTypeDetection::isAudioFormat(mime);
    const bool is_video = FileTypeDetection::isVideoFormat(mime);
    const bool is_pdf = FileTypeDetection::isPdf(mime);

    if (is_pdf)
    {
        PdfViewWindow* viewer = new PdfViewWindow(this);
        if (!viewer->loadPdfFile(selectedFile))
        {
            QMessageBox::critical(
                this, tr("Error while opening the file"),
                tr("The file '%1' could not be opened for reading.")
                    .arg(selectedFile));
            delete viewer;
            return;
        }

        viewer->setWindowModality(Qt::WindowModality::WindowModal);
        viewer->show();

        // show() returns immediately, so the deletion of viewer is handled by the
        // PdfViewWindow itself in its closeEvent();
    }
    else if (is_audio)
    {
        // load as audio file
        AudioPlayerWindow* viewer = new AudioPlayerWindow(this);
        if (!viewer->loadAudioFile(selectedFile))
        {
            QMessageBox::critical(
                this, tr("Error while opening the file"),
                tr("The file '%1' could not be opened for reading.")
                    .arg(selectedFile));
            delete viewer;
            return;
        }
        viewer->setAutoPlay(settings.getAutoPlayAudio());
        viewer->setLoopForever(settings.getLoopAudioForever());
        viewer->setVolume(settings.getAudioVolume());
        viewer->setWindowModality(Qt::WindowModality::WindowModal);
        viewer->show();

        // show() returns immediately, so the deletion of viewer is handled by the
        // AudioPlayerWindow itself in its closeEvent();
    }
    else if (is_video)
    {
        VideoPlayerWindow* viewer = new VideoPlayerWindow(this);
        if (!viewer->loadVideoFile(selectedFile))
        {
            QMessageBox::critical(
                this, tr("Error while opening the file"),
                tr("The file '%1' could not be opened for reading.")
                    .arg(selectedFile));
            delete viewer;
            return;
        }
        viewer->setAutoPlay(settings.getAutoPlayVideo());
        viewer->setLoopForever(settings.getLoopVideoForever());
        viewer->setVolume(settings.getVideoVolume());
        viewer->setWindowModality(Qt::WindowModality::WindowModal);
        viewer->show();

        // show() returns immediately, so the deletion of viewer is handled by the
        // VideoPlayerWindow itself in its closeEvent();
    }
    else if (is_supported_movie)
    {
        // load as movie
        AnimationViewWindow* viewer = new AnimationViewWindow(this);
        if (!viewer->loadMovieFile(selectedFile))
        {
            QMessageBox::critical(
                this, tr("Error while opening the file"),
                tr("The file '%1' could not be opened for reading.")
                    .arg(selectedFile));
            delete viewer;
            return;
        }
        viewer->setWindowModality(Qt::WindowModality::WindowModal);
        viewer->show();

        // show() returns immediately, so the deletion of viewer is handled by the
        // AnimationViewWindow itself in its closeEvent();
    }
    else if (is_supported_image)
    {
        // load as image
        ImageViewWindow* viewer = new ImageViewWindow(this);
        if (!viewer->loadImageFile(selectedFile))
        {
            QMessageBox::critical(
                this, tr("Error while opening the file"),
                tr("The file '%1' could not be opened for reading.")
                    .arg(selectedFile));
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
                this, tr("Error while opening the file"),
                tr("The file '%1' could not be opened for reading.")
                    .arg(selectedFile));
            delete viewer;
            return;
        }
        viewer->setFont(settings.getTextViewerFont());
        viewer->setHightlighterTheme(settings.getTextViewerHightlightingTheme());
        viewer->setAutoSelectLanguage(settings.getTextViewerAutoSelectLanguage());
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
            this, tr("No file selected"),
            tr("No file or directory has been selected whose properties could be displayed."));
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
        statusBar()->showMessage(tr("View of directory '") + path + "' ("
                                     + (treeWidget == ui->treeWidgetLeft ? tr("left") : tr("right"))
                                     + tr(") has been updated."), 5000);
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

    connect(ui->btnGoUpLeft, &QPushButton::clicked, this, &MainWindow::btnGoUpClicked);
    connect(ui->btnGoHomeLeft, &QPushButton::clicked, this, &MainWindow::btnGoHomeClicked);
    connect(ui->btnGoUpRight, &QPushButton::clicked, this, &MainWindow::btnGoUpClicked);
    connect(ui->btnGoHomeRight, &QPushButton::clicked, this, &MainWindow::btnGoHomeClicked);
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

    languageGroup.addAction(ui->actionLanguageEnglish);
    languageGroup.addAction(ui->actionLanguageGerman);
    languageGroup.addAction(ui->actionLanguageDutch);
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
                               || (settings.getUseProvidedFileIcons() != this->settings.getUseProvidedFileIcons())
                               || (settings.getShowFormattedSize() != this->settings.getShowFormattedSize()));

    // New values for filters and sort flags need to be set before a potential
    // refresh happens, because the refresh uses the currently set flags.
    this->settings = settings;

    if (needs_refresh && !avoidRefresh)
    {
        refreshBothViews();
    }
}

void MainWindow::changeLanguage(const QLocale &new_locale, QAction* action)
{
    QApplication::removeTranslator(&translator);

    if (!translator.load(new_locale, "", translatorPrefix, translatorDirectory))
    {
        QMessageBox::critical(this, tr("Failure"),
                              tr("Could not load %1 language data.")
                                  .arg(new_locale.languageToString(new_locale.language())));
        action->setChecked(false);
        return;
    }
    if (!QApplication::installTranslator(&translator))
    {
        QMessageBox::critical(this, tr("Failure"),
                              tr("Could not install %1 language translator.")
                                  .arg(new_locale.languageToString(new_locale.language())));
        action->setChecked(false);
        return;
    }

    ui->retranslateUi(this);
    refreshBothViews();
}

void MainWindow::attemptToLoadMatchingTranslation()
{
    const QLocale::Language lang = locale().language();
    if ((lang != QLocale::Language::German) && (lang != QLocale::Language::Dutch))
    {
        return;
    }

    if (!translator.load(locale(), "", translatorPrefix, translatorDirectory))
    {
        return;
    }
    if (!QApplication::installTranslator(&translator))
    {
        return;
    }

    ui->retranslateUi(this);
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
    connect(ui->actionShowFormattedSize, &QAction::triggered, this, &MainWindow::actionShowFormattedSizeTriggered);

    connect(ui->actionRefresh, &QAction::triggered, this, &MainWindow::actionRefreshTriggered);

    connect(ui->actionCalculateChecksum, &QAction::triggered, this, &MainWindow::actionCalculateChecksumTriggered);
    connect(ui->actionCompareDirectories, &QAction::triggered, this, &MainWindow::actionCompareDirectoriesTriggered);

    connect(ui->actionEditSettings, &QAction::triggered, this, &MainWindow::actionEditSettingsTriggered);
    connect(ui->actionSaveSettings, &QAction::triggered, this, &MainWindow::actionSaveSettingsTriggered);
    connect(ui->actionLoadSettings, &QAction::triggered, this, &MainWindow::actionLoadSettingsTriggered);
    connect(ui->actionRestoreDefaultSettings, &QAction::triggered, this, &MainWindow::actionRestoreDefaultSettingsTriggered);

    connect(ui->actionLanguageEnglish, &QAction::triggered, this, &MainWindow::actionLanguageEnglishTriggered);
    connect(ui->actionLanguageGerman, &QAction::triggered, this, &MainWindow::actionLanguageGermanTriggered);
    connect(ui->actionLanguageDutch, &QAction::triggered, this, &MainWindow::actionLanguageDutchTriggered);

    connect(ui->actionShowMountpoints, &QAction::triggered, this, &MainWindow::actionShowMountpointsTriggered);
    connect(ui->actionAboutQtCommander, &QAction::triggered, this, &MainWindow::actionAboutQtCmdrTriggered);
    connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindow::actionAboutQtTriggered);
}

void MainWindow::actionRefreshTriggered()
{
    refreshCurrentView();
}

void MainWindow::actionCalculateChecksumTriggered()
{
    QTreeWidget* treeWidget = latestTreeWidget();
    const QList<QTreeWidgetItem*> selection = treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        QMessageBox::information(
            this, tr("No file selected"),
            tr("No file has been selected. However, a file must be selected to calculate the checksum."));
        return;
    }
    QTreeWidgetItem* item = selection.at(0);
    const QString name = item->text(0);

    const QString selectedFile = currentDirectory().absoluteFilePath(name);
    const QFileInfo info(selectedFile);

    if (!info.isFile())
    {
#if defined(__linux__)
        const QString title = tr("No suitable file selected");
        const QString message =
            tr("No file has been selected for the calculation.") + "\n\n"
            + tr("Checksums can only be calculated for files, not for directories.")
            + tr(" Special file types like block devices, FIFOs or sockets are not supported either.");
#else
        const QString title = QStringLiteral("Keine Datei ausgewählt");
        const QString message = tr("No file has been selected for the calculation.") + "\n\n"
            + tr("Checksums can only be calculated for files, not for directories.");
#endif
        QMessageBox::information(this, title, message);
        return;
    }

    CheckSumDialog dialog(selectedFile, this);
    dialog.setPreselectedAlgorithm(settings.getSelectedHashAlgorithm());
    dialog.exec();
    if (dialog.hasCreatedNewFiles())
    {
        if (DirUtils::isSameDir(currentDirectory(), otherDirectory()))
        {
            refreshBothViews();
        }
        else
        {
            refreshCurrentView();
        }
    }
}

void MainWindow::actionCompareDirectoriesTriggered()
{
    if (DirUtils::isSameDir(currentDirectoryLeft, currentDirectoryRight))
    {
        QMessageBox::information(
            this, tr("Same directory"),
            QString(tr("The left and the right view show the same directory."))
                + tr(" Therefore, a comparison is useless, because the contents are identical.")
            );
        return;
    }

    DirectoryCompareWindow* window = new DirectoryCompareWindow(
        currentDirectoryLeft.absolutePath(), currentDirectoryRight.absolutePath(),
        settings.getCompareCaseSensitivity(), this);
    window->show();
}

void MainWindow::actionEditSettingsTriggered()
{
    SettingsDialog dialog(settings, this);
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    putSettingsIntoGui(dialog.selectedSettings());
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

void MainWindow::actionLanguageEnglishTriggered(bool checked)
{
    if (!checked)
    {
        return;
    }

    const QLocale new_locale(QLocale::Language::English, QLocale::Territory::UnitedStates);
    changeLanguage(new_locale, ui->actionLanguageEnglish);
}

void MainWindow::actionLanguageGermanTriggered(bool checked)
{
    if (!checked)
    {
        return;
    }

    const QLocale new_locale(QLocale::Language::German, QLocale::Territory::Germany);
    changeLanguage(new_locale, ui->actionLanguageGerman);
}

void MainWindow::actionLanguageDutchTriggered(bool checked)
{
    if (!checked)
    {
        return;
    }

    const QLocale new_locale(QLocale::Language::Dutch, QLocale::Territory::Netherlands);
    changeLanguage(new_locale, ui->actionLanguageDutch);
}

void MainWindow::actionShowMountpointsTriggered()
{
    MountedVolumesDialog dialog(this);
    dialog.exec();
}

void MainWindow::actionAboutQtCmdrTriggered()
{
    qtcmdr::GitInfos info;
    QString message = QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion()
                      + "\n\nVersion control commit: " + QString::fromStdString(info.commit().substr(0, 7))
                      + "\nVersion control date: " + QString::fromStdString(info.date());
    message += QString("\n\nCopyright (C) 2026  Dirk Stolle\n\n")
               + "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n\n"
               + "This is free software: you are free to change and redistribute it under the "
               + "terms of the GNU General Public License version 3 or any later version.\n"
               + "There is NO WARRANTY, to the extent permitted by law.";
    QMessageBox::about(this, QCoreApplication::applicationName(), message);
}

void MainWindow::actionAboutQtTriggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::actionShowHiddenFilesTriggered(bool checked)
{
    if (checked)
    {
        settings.setFilters(settings.getFilters() | QDir::Filter::Hidden);
    }
    else
    {
        settings.setFilters(settings.getFilters() ^ QDir::Filter::Hidden);
    }
    refreshBothViews();
}

void MainWindow::actionShowSystemFilesTriggered(bool checked)
{
    if (checked)
    {
        settings.setFilters(settings.getFilters() | QDir::Filter::System);
    }
    else
    {
        settings.setFilters(settings.getFilters() ^ QDir::Filter::System);
    }
    refreshBothViews();
}

void MainWindow::actionHideFilesTriggered(bool checked)
{
    if (checked)
    {
        settings.setFilters(settings.getFilters() ^ QDir::Filter::Files);
    }
    else
    {
        settings.setFilters(settings.getFilters() | QDir::Filter::Files);
    }
    refreshBothViews();
}

void MainWindow::actionSortBySomethingTriggered(bool checked)
{
    if (!checked)
    {
        return;
    }

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
    refreshBothViews();
}

void MainWindow::actionReverseSortTriggered(bool checked)
{
    if (checked)
    {
        settings.setSortFlags(settings.getSortFlags() | QDir::SortFlag::Reversed);
    }
    else
    {
        settings.setSortFlags(settings.getSortFlags() ^ QDir::SortFlag::Reversed);
    }
    refreshBothViews();
}

void MainWindow::actionSortIgnoreCaseTriggered(bool checked)
{
    if (checked)
    {
        settings.setSortFlags(settings.getSortFlags() | QDir::SortFlag::IgnoreCase);
    }
    else
    {
        settings.setSortFlags(settings.getSortFlags() ^ QDir::SortFlag::IgnoreCase);
    }
    refreshBothViews();
}

void MainWindow::actionSortSomethingFirstTriggered(bool checked)
{
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
    refreshBothViews();
}

void MainWindow::actionUseProvidedIconsTriggered(bool checked)
{
    settings.setUseProvidedFileIcons(checked);
    refreshBothViews();
}

void MainWindow::actionShowFormattedSizeTriggered(bool checked)
{
    settings.setShowFormattedSize(checked);
    refreshBothViews();
}
