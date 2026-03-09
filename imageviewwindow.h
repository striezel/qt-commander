#ifndef IMAGEVIEWWINDOW_H
#define IMAGEVIEWWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QPixmap>

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
protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
private slots:
    void actionSupportedFileFormatsTriggered();
private:
    Ui::ImageViewWindow *ui;

    QPixmap* loadedPixmap;

    void rescaleToFit();
};

#endif // IMAGEVIEWWINDOW_H
