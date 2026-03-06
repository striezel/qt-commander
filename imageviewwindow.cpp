#include "imageviewwindow.h"
#include "ui_imageviewwindow.h"

ImageViewWindow::ImageViewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ImageViewWindow)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &ImageViewWindow::close);
}

ImageViewWindow::~ImageViewWindow()
{
    delete ui;
}

bool ImageViewWindow::loadImageFile(const QString &path)
{
    QPixmap pixmap(path);
    if (pixmap.isNull())
    {
        return false;
    }

    ui->label->setPixmap(pixmap);

    this->setWindowTitle("Bildbetrachter - " + path);

    return true;
}
