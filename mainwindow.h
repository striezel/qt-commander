#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settings.h"

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

private slots:
    void treeItemDoubleClicked(QTreeWidgetItem* item, int column);

    void btnRemoveClicked();
    void btnCreateDirectoryClicked();
    void btnMoveClicked();
    void btnCopyClicked();
    void btnViewClicked();

    void actionShowHiddenFilesTriggered(bool checked = false);
    void actionShowSystemFilesTriggered(bool checked = false);
    void actionHideFilesTriggered(bool checked = false);

    void actionSortBySomethingTriggered(bool checked = false);
    void actionReverseSortTriggered(bool checked = false);

    void actionSortIgnoreCaseTriggered(bool checked = false);

    void actionSortSomethingFirstTriggered(bool checked = false);

    void actionRefreshTriggered();

    void actionSaveSettingsTriggered();
    void actionLoadSettingsTriggered();

private:
    Ui::MainWindow *ui;

    QDir currentDirectoryLeft;
    QDir currentDirectoryRight;

    /// current filters for shown files/directories - applies to both views
    QDir::Filters filters;

    /// current sort flags for shown files/directories - applies to both views
    QDir::SortFlags sortFlags;

    QActionGroup sortActionGroup;
    QActionGroup whatFirstGroup;

    void fillTreeWidget(QTreeWidget* treeWidget, const QString& path);

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

    void putSettingsIntoGui(const Settings& settings);
};
#endif // MAINWINDOW_H
