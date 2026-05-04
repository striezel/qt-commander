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

#include "imageviewwindow.h"
#include "ui_imageviewwindow.h"

#include <QImageReader>
#include <QMessageBox>

ImageViewWindow::ImageViewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ImageViewWindow)
    , loadedPixmap(nullptr)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &ImageViewWindow::close);
    connect(ui->actionSupportedFileFormats, &QAction::triggered,
            this, &ImageViewWindow::actionSupportedFileFormatsTriggered);
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
    Q_UNUSED(event);

    rescaleToFit();
}

void ImageViewWindow::resizeEvent(QResizeEvent *event)
{
    if (this->isVisible())
    {
        rescaleToFit();
    }
    this->QMainWindow::resizeEvent(event);
}

QString ImageViewWindow::supportedFormatsMessage()
{
    QString message = "Der Bildbetrachter unterstützt folgende MIME-Typen:\n";
    const QList<QByteArray> supported_types = QImageReader::supportedMimeTypes();
    for (const QByteArray& element: supported_types)
    {
        message = message + "\n" + element;
    }
    message += "\n\nDie Unterstützung kann je nach System und installierten Qt-Modulen variieren.";
    if (!supported_types.contains("image/webp")
        || !supported_types.contains("image/tiff")
        || !supported_types.contains("image/x-tga"))
    {
        message += QString(" Durch Installation des Qt Image Formats Add-Ons kann")
                   + " unter anderem die Unterstützung für WebP, TGA und TIFF erreicht werden.";
    }
    if (!supported_types.contains("image/svg+xml"))
    {
        message += " Durch Installation des Qt-SVG-Moduls kann die Unterstützung für SVG-Dateien hinzugefügt werden.";
    }
    return message;
}

void ImageViewWindow::actionSupportedFileFormatsTriggered()
{
    QMessageBox::about(this, "Unterstützte Dateiformate", supportedFormatsMessage());
}

void ImageViewWindow::rescaleToFit()
{
    if (loadedPixmap == nullptr)
    {
        return;
    }

    const QSize img_size = loadedPixmap->size();
    const QSize lbl_size = ui->label->size();
    if (img_size.width() > lbl_size.width() || img_size.height() > lbl_size.height())
    {
        QPixmap rescaled = loadedPixmap->scaled(lbl_size, Qt::AspectRatioMode::KeepAspectRatio);
        if (rescaled.isNull())
        {
            return;
        }
        const QSize re_size = rescaled.size();
        ui->label->setPixmap(rescaled);
        double percentage = img_size.height() > img_size.width()
                                ? (re_size.height() * 100.0 / img_size.height())
                                : (re_size.width() * 100.0 / img_size.width());
        percentage = std::round(percentage * 10.0) / 10.0;
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
