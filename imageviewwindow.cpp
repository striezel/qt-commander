#include "imageviewwindow.h"
#include "ui_imageviewwindow.h"

ImageViewWindow::ImageViewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ImageViewWindow)
    , loadedPixmap(nullptr)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &ImageViewWindow::close);
}

ImageViewWindow::~ImageViewWindow()
{
    delete loadedPixmap;
    loadedPixmap = nullptr;

    delete ui;
}

bool ImageViewWindow::loadImageFile(const QString &path)
{
    QPixmap* new_pixmap = new QPixmap(path);
    if (new_pixmap->isNull())
    {
        return false;
    }
    qDebug() << "new pixmap size: " << new_pixmap->size();
    qDebug() << "label size:      " << ui->label->size();

    ui->label->setPixmap(*new_pixmap);

    this->setWindowTitle("Bildbetrachter - " + path);
    delete loadedPixmap;
    loadedPixmap = new_pixmap;
    rescaleToFit();

    return true;
}

void ImageViewWindow::closeEvent(QCloseEvent *event)
{
    // ensure deletion
    this->deleteLater();

    // do the usual event handling inherited from base class
    this->QMainWindow::closeEvent(event);
}

void ImageViewWindow::showEvent(QShowEvent *event)
{
    qDebug() << "ImageViewWindow::showEvent";
    rescaleToFit();
}

void ImageViewWindow::resizeEvent(QResizeEvent *event)
{
    qDebug() << "ImageViewWindow::resizeEvent";
    rescaleToFit();
    this->QMainWindow::resizeEvent(event);
}

void ImageViewWindow::rescaleToFit()
{
    if (loadedPixmap == nullptr)
    {
        return;
    }

    const QSize img_size = loadedPixmap->size();
    const QSize lbl_size = ui->label->size();
    qDebug() << "pixmap size: " << img_size;
    qDebug() << "label size:  " << lbl_size;
    if (img_size.width() > lbl_size.width() || img_size.height() > lbl_size.height())
    {
        QPixmap rescaled = loadedPixmap->scaled(lbl_size, Qt::AspectRatioMode::KeepAspectRatio);
        if (rescaled.isNull())
        {
            return;
        }
        qDebug() << "rescaled size:" << rescaled.size();
        ui->label->setPixmap(rescaled);
        double percentage = lbl_size.height() > lbl_size.width()
                                ? (lbl_size.height() * 100.0 / img_size.height())
                                : (lbl_size.width() * 100.0 / img_size.width());
        qDebug() << "percentage: " << percentage;
        percentage = std::round(percentage * 10.0) / 10.0;
        qDebug() << "percentage rounded: " << percentage;
        ui->statusbar->showMessage("Bildgröße: " + QString::number(img_size.width())
                                   + " x " + QString::number(img_size.height())
                                   + " Pixel, skaliert auf " + QString::number(percentage) + " %");
    }
    else
    {
        // label is big enough to fit the pixmap, so use it directly
        ui->label->setPixmap(*loadedPixmap);
        ui->statusbar->showMessage("Bildgröße: " + QString::number(img_size.width())
                                   + " x " + QString::number(img_size.height())
                                   + " Pixel");
    }
}
