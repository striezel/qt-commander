#ifndef FILEINFOWINDOW_H
#define FILEINFOWINDOW_H

#include <QMainWindow>

namespace Ui {
class FileInfoWindow;
}

class FileInfoWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileInfoWindow(QWidget *parent = nullptr);
    ~FileInfoWindow();

    void loadInformation(const QString& filePath);
protected:
    void closeEvent(QCloseEvent* event) override;
private:
    Ui::FileInfoWindow *ui;
};

#endif // FILEINFOWINDOW_H
