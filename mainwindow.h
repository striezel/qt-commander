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

    void btnCreateDirectoryClicked();

private:
    Ui::MainWindow *ui;

    QDir currentDirectoryLeft;
    QDir currentDirectoryRight;

    void fillTreeWidget(QTreeWidget& treeWidget, const QString& path);

    /// Checks whether the left tree widget is the latest tree widget to get
    /// the focus.
    bool leftTreeIsLatest() const;
};
#endif // MAINWINDOW_H
