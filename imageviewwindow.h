#ifndef IMAGEVIEWWINDOW_H
#define IMAGEVIEWWINDOW_H

#include <QMainWindow>

namespace Ui {
class ImageViewWindow;
}

class ImageViewWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ImageViewWindow(QWidget *parent = nullptr);
    ~ImageViewWindow();

    /// Loads the file specified by path as image file.
    ///
    /// Returns true, if image was successfully loaded.
    /// Returns false otherwise.
    bool loadImageFile(const QString& path);

private:
    Ui::ImageViewWindow *ui;
};

#endif // IMAGEVIEWWINDOW_H
