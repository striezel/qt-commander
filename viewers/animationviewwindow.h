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

#ifndef ANIMATIONVIEWWINDOW_H
#define ANIMATIONVIEWWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QMovie>
#include <QString>

namespace Ui {
class AnimationViewWindow;
}

class AnimationViewWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AnimationViewWindow(QWidget *parent = nullptr);
    ~AnimationViewWindow();

    /// Loads the file specified by path as movie file.
    ///
    /// Returns true, if movie was successfully loaded.
    /// Returns false otherwise.
    bool loadMovieFile(const QString& path);

    /// Gets a message describing the supported file formats.
    static QString supportedFormatsMessage();
protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
private slots:
    void actionSupportedFileTypesTriggered();

private:
    Ui::AnimationViewWindow *ui;

    QMovie* movie;

    void startAnimation();
};

#endif // ANIMATIONVIEWWINDOW_H
