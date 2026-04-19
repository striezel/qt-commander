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
    connect(ui->actionAutoStartVideos, &QAction::triggered,
            this, &AnimationViewWindow::actionAutoStartVideosTriggered);
    connect(ui->actionSupportedFileTypes, &QAction::triggered,
            this, &AnimationViewWindow::actionSupportedFileTypesTriggered);

    connect(ui->btnStart, &QPushButton::clicked, this, &AnimationViewWindow::btnStartClicked);
    connect(ui->btnStop, &QPushButton::clicked, this, &AnimationViewWindow::btnStopClicked);
    connect(ui->btnPause, &QPushButton::clicked, this, &AnimationViewWindow::btnPauseClicked);

    if (parent != nullptr)
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent);
        connect(this, &AnimationViewWindow::autoStartChanged, castedParent, &MainWindow::movieViewerAutoStartChanged);
    }
}

AnimationViewWindow::~AnimationViewWindow()
{
    if (movie != nullptr)
    {
        movie->stop();
        delete movie;
        movie = nullptr;
    }

    if (parent() != nullptr)
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent());
        disconnect(this, &AnimationViewWindow::autoStartChanged, castedParent, &MainWindow::movieViewerAutoStartChanged);
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

void AnimationViewWindow::setAutoStartVideos(const bool autoStart)
{
    ui->actionAutoStartVideos->setChecked(autoStart);
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
    if (ui->actionAutoStartVideos->isChecked())
    {
        btnStartClicked();
    }
}

void AnimationViewWindow::btnStartClicked()
{
    if (movie == nullptr)
    {
        return;
    }

    movie->start();
}

void AnimationViewWindow::btnStopClicked()
{
    if (movie == nullptr)
    {
        return;
    }

    movie->stop();
}

void AnimationViewWindow::btnPauseClicked()
{
    if (movie == nullptr)
    {
        return;
    }

    movie->setPaused(movie->state() != QMovie::MovieState::Paused);
}

void AnimationViewWindow::actionAutoStartVideosTriggered(bool checked)
{
    emit autoStartChanged(checked);
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
    message += "\n\nUnterstützung kann je nach System unterschiedlich ausfallen.";
    return message;
}

void AnimationViewWindow::actionSupportedFileTypesTriggered()
{
    QMessageBox::about(this, "Unterstützte Dateiformate", supportedFormatsMessage());
}
