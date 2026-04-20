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

#include "mainwindow.h"
#include "animationviewwindow.h"
#include "ui_animationviewwindow.h"

#include <QMessageBox>

AnimationViewWindow::AnimationViewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AnimationViewWindow)
    , movie(nullptr)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &AnimationViewWindow::close);
    connect(ui->actionSupportedFileTypes, &QAction::triggered,
            this, &AnimationViewWindow::actionSupportedFileTypesTriggered);
}

AnimationViewWindow::~AnimationViewWindow()
{
    if (movie != nullptr)
    {
        movie->stop();
        delete movie;
        movie = nullptr;
    }

    delete ui;
}

bool AnimationViewWindow::loadMovieFile(const QString &path)
{
    QMovie* new_movie = new QMovie(path);
    if (!new_movie->isValid())
    {
        return false;
    }

    ui->label->setMovie(new_movie);

    setWindowTitle("Animationsbetrachter - " + path);
    if (movie != nullptr)
    {
        if (movie->state() == QMovie::MovieState::Running)
        {
            movie->stop();
        }
        delete movie;
        movie = nullptr;
    }
    movie = new_movie;

    return true;
}

void AnimationViewWindow::closeEvent(QCloseEvent *event)
{
    // ensure deletion
    this->deleteLater();

    // do the usual event handling inherited from base class
    this->QMainWindow::closeEvent(event);
}

void AnimationViewWindow::showEvent(QShowEvent *event)
{
    // automatically start animation
    startAnimation();
}

void AnimationViewWindow::startAnimation()
{
    if (movie == nullptr)
    {
        return;
    }

    movie->start();
}

QString AnimationViewWindow::supportedFormatsMessage()
{
    QString message = "Der Animationsbetrachter unterstützt folgende Dateitypen:\n";
    const QList<QByteArray> formats = QMovie::supportedFormats();
    for (const QByteArray& element: formats)
    {
        message = message + "\n" + element;
    }
    if (formats.empty())
    {
        message += "\nKeine. Das ist schlecht.";
    }
    message += "\n\nUnterstützung kann je nach System und installierten Qt-Modulen unterschiedlich ausfallen.";
    if (!formats.contains("mng") || !formats.contains("webp"))
    {
        message += QString(" Durch Installation des Qt Image Formats Add-Ons kann")
        + " unter anderem die Unterstützung für MNG und WebP erreicht werden.";
    }
    return message;
}

void AnimationViewWindow::actionSupportedFileTypesTriggered()
{
    QMessageBox::about(this, "Unterstützte Dateiformate", supportedFormatsMessage());
}
