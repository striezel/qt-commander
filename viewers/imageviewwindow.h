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
