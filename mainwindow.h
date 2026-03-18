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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "util/filetypedetection.h"
#include "util/settings.h"

#include <QMainWindow>
#include <QActionGroup>
#include <QDir>
#include <QString>
#include <QTreeWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void textViewerFontChanged(const QFont& new_font);
    void movieViewerAutoStartChanged(const bool autoStart);
    void audioPlayerAutoPlayChanged(const bool autoPlay);
    void audioPlayerVolumeChanged(const int volume);
    void changeLeftTree(const QString& newPath);
    void changeRightTree(const QString& newPath);

private slots:
    void treeItemDoubleClicked(QTreeWidgetItem* item, int column);
    void leftTreeWidgetActivated(const QModelIndex& idx);
    void rightTreeWidgetActivated(const QModelIndex& idx);

    void btnRemoveClicked();
    void btnCreateDirectoryClicked();
    void btnMoveClicked();
    void btnCopyClicked();
    void btnViewClicked();

    void actionShowFileInfoTriggered();

    void actionShowHiddenFilesTriggered(bool checked = false);
    void actionShowSystemFilesTriggered(bool checked = false);
    void actionHideFilesTriggered(bool checked = false);

    void actionSortBySomethingTriggered(bool checked = false);
    void actionReverseSortTriggered(bool checked = false);

    void actionSortIgnoreCaseTriggered(bool checked = false);

    void actionSortSomethingFirstTriggered(bool checked = false);

    void actionUseProvidedIconsTriggered(bool checked = false);

    void actionRefreshTriggered();

    void actionSaveSettingsTriggered();
    void actionLoadSettingsTriggered();
    void actionRestoreDefaultSettingsTriggered();

    void actionShowMountpointsTriggered();
    void actionAboutQtCmdrTriggered();
    void actionAboutQtTriggered();

private:
    Ui::MainWindow *ui;

    QDir currentDirectoryLeft;
    QDir currentDirectoryRight;

    Settings settings;

    QActionGroup sortActionGroup;
    QActionGroup whatFirstGroup;

    FileTypeDetection detection;

    void fillTreeWidget(QTreeWidget* treeWidget, const QString& path, const bool selectFirst = false);

    /// Checks whether the left tree widget is the latest tree widget to get
    /// the focus.
    bool leftTreeIsLatest() const;

    /// Returns the latest tree widget, i.e. the left or the right one.
    QTreeWidget* latestTreeWidget() const;

    /// Returns the "other" tree widget, i. e. the one which is not the latest.
    QTreeWidget* otherTreeWidget() const;

    /// Returns a reference to the current directory of the "latest" tree widget.
    const QDir& currentDirectory() const;

    /// Returns a reference to the current directory of the "other" tree widget.
    const QDir& otherDirectory() const;

    /// Refreshes the current (latest) view.
    void refreshCurrentView();

    /// Refreshes both views.
    void refreshBothViews();

    /// Refreshes the given tree widget.
    ///
    /// @param treeWidget  the widget to refresh - must not be nullptr
    /// @param dir         the directory which shall be listed in the tree widget
    /// @param showStatusMessage  whether to show a message about the refresh in the status bar
    void refreshView(QTreeWidget* treeWidget, const QDir& dir, const bool showStatusMessage = true);

    /// Creates connections between signals and slots for buttons in lower half of the window.
    void connectButtons();

    /// Creates connections between signals and slots for menu actions.
    void connectMenuActions();

    void setUpActionGroups();

    void putSettingsIntoGui(const Settings& settings, const bool avoidRefresh = false);
};
#endif // MAINWINDOW_H
