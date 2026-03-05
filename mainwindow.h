#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

    void actionRefreshTriggered();
    void actionShowHiddenFilesTriggered(bool checked = false);
    void actionShowSystemFilesTriggered(bool checked = false);
    void actionHideFilesTriggered(bool checked = false);

private:
    Ui::MainWindow *ui;

    QDir currentDirectoryLeft;
    QDir currentDirectoryRight;

    QDir::Filters filters;

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

    void refreshCurrentView();
    void refreshBothViews();
    void refreshView(QTreeWidget* treeWidget, const QDir& dir, const bool showStatusMessage = true);
};
#endif // MAINWINDOW_H
