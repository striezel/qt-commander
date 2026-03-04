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

private:
    Ui::MainWindow *ui;

    QDir currentDirectoryLeft;
    QDir currentDirectoryRight;

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

    /// Checks whether the first QDir is a parent directory of the second QDir.
    /// If both QDir instances point to the same directory, then that counts as
    /// parent, too, although that's not a parent in the strict sense.
    static bool isParentOf(const QDir& parent, const QDir& potentialChild);

    /// Checks whether both QDir instances point to the same directory.
    ///
    /// The function is necessary, because the equality operator of QDir also
    /// checks for sort order and filters during comparison, but this is
    /// irrelevant. Only the location in the filesystem is relevant.
    static bool isSameDir(const QDir& one, const QDir& two);
};
#endif // MAINWINDOW_H
