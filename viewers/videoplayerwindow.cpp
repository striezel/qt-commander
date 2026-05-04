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

#include "videoplayerwindow.h"
#include "ui_videoplayerwindow.h"
#include "../mainwindow.h"

#include <algorithm> // for std::clamp()
#include <QMediaFormat>
#include <QMediaMetaData>
#include <QMessageBox>

#include "util/durationutils.h"

VideoPlayerWindow::VideoPlayerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VideoPlayerWindow)
    , mediaPlayer(new QMediaPlayer(this))
    , audioOutput(new QAudioOutput())
    , mediaDurationMillis(-1)
{
    ui->setupUi(this);
    ui->videoWidget->setAspectRatioMode(Qt::AspectRatioMode::KeepAspectRatio);

    connect(ui->actionClose, &QAction::triggered, this, &VideoPlayerWindow::close);

    connect(ui->btnStart, &QPushButton::clicked, this, &VideoPlayerWindow::btnStartClicked);
    connect(ui->btnStop, &QPushButton::clicked, this, &VideoPlayerWindow::btnStopClicked);
    connect(ui->btnPause, &QPushButton::clicked, this, &VideoPlayerWindow::btnPauseClicked);

    // Using &QSlider::sliderMoved instead of &QSlider::valueChanged for position,
    // because otherwise the setting of a new value in code would trigger this
    // signal again, resulting in potentially endless loops.
    connect(ui->sliderPosition, &QSlider::sliderMoved, this, &VideoPlayerWindow::sliderPositionMoved);
    connect(ui->sliderVolume, &QSlider::valueChanged, this, &VideoPlayerWindow::sliderVolumeValueChanged);

    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoPlayerWindow::durationChanged);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayerWindow::positionChanged);
    connect(mediaPlayer, &QMediaPlayer::seekableChanged, this, &VideoPlayerWindow::seekableChanged);
    connect(mediaPlayer, &QMediaPlayer::errorOccurred, this, &VideoPlayerWindow::mediaErrorOccurred);

    connect(ui->actionAutoPlayVideo, &QAction::triggered, this, &VideoPlayerWindow::actionAutoPlayVideoTriggered);
    connect(ui->actionLoopForever, &QAction::triggered, this, &VideoPlayerWindow::actionLoopForeverTriggered);
    connect(ui->actionSupportedFormats, &QAction::triggered, this, &VideoPlayerWindow::actionSupportedFormatsTriggered);
    connect(ui->actionShowMetadata, &QAction::triggered, this, &VideoPlayerWindow::actionShowMetadataTriggered);

    // set initial volume to whatever the slider shows currently
    sliderVolumeValueChanged(ui->sliderVolume->value());

    if (parent != nullptr)
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent);
        connect(this, &VideoPlayerWindow::autoPlayChanged, castedParent, &MainWindow::videoPlayerAutoPlayChanged);
        connect(this, &VideoPlayerWindow::loopForeverChanged, castedParent, &MainWindow::videoPlayerLoopForeverChanged);
        connect(this, &VideoPlayerWindow::videoVolumeChanged, castedParent, &MainWindow::videoPlayerVolumeChanged);
    }
}

VideoPlayerWindow::~VideoPlayerWindow()
{
    btnStopClicked();

    delete mediaPlayer;
    mediaPlayer = nullptr;
    delete audioOutput;
    audioOutput = nullptr;

    if (parent() != nullptr)
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent());
        disconnect(this, &VideoPlayerWindow::autoPlayChanged, castedParent, &MainWindow::videoPlayerAutoPlayChanged);
        disconnect(this, &VideoPlayerWindow::loopForeverChanged, castedParent, &MainWindow::videoPlayerLoopForeverChanged);
        disconnect(this, &VideoPlayerWindow::videoVolumeChanged, castedParent, &MainWindow::videoPlayerVolumeChanged);
    }

    delete ui;
}

bool VideoPlayerWindow::loadVideoFile(const QString &path)
{
    if (mediaPlayer->playbackState() == QMediaPlayer::PlaybackState::PlayingState)
    {
        mediaPlayer->stop();
    }

    mediaPlayer->setSource(QUrl::fromLocalFile(path));
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setVideoOutput(ui->videoWidget);

    setWindowTitle("Videowiedergabe - " + path);

    const QMediaMetaData metaData = mediaPlayer->metaData();
    QVariant duration = metaData.value(QMediaMetaData::Duration);
    bool ok = true;
    qint64 durationMillis = duration.isNull() ? -1 : duration.toLongLong(&ok);
    if (!ok)
    {
        durationMillis = -1;
    }

    if (durationMillis < 0)
    {
        ui->sliderPosition->setEnabled(false);
    }
    else
    {
        ui->sliderPosition->setMinimum(0);
        ui->sliderPosition->setValue(0);
        ui->sliderPosition->setMaximum(durationMillis / 1000);
        ui->sliderPosition->setEnabled(true);
    }
    this->mediaDurationMillis = durationMillis;
    showPosition(0, mediaDurationMillis);

    return true;
}

void VideoPlayerWindow::setAutoPlay(const bool autoPlay)
{
    ui->actionAutoPlayVideo->setChecked(autoPlay);
}

void VideoPlayerWindow::setLoopForever(const bool loopForever)
{
    ui->actionLoopForever->setChecked(loopForever);
}

void VideoPlayerWindow::setVolume(const int volume)
{
    ui->sliderVolume->setValue(std::clamp(volume, 0, 100));
}

void VideoPlayerWindow::closeEvent(QCloseEvent *event)
{
    // ensure deletion
    this->deleteLater();

    // do the usual event handling inherited from base class
    this->QMainWindow::closeEvent(event);
}

void VideoPlayerWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);

    if (ui->actionAutoPlayVideo->isChecked())
    {
        btnStartClicked();
    }
    if (ui->actionLoopForever->isChecked() && mediaPlayer != nullptr)
    {
        mediaPlayer->setLoops(QMediaPlayer::Loops::Infinite);
    }
}

void VideoPlayerWindow::btnStartClicked()
{
    if (mediaPlayer == nullptr)
    {
        return;
    }

    mediaPlayer->play();
}

void VideoPlayerWindow::btnStopClicked()
{
    if (mediaPlayer == nullptr)
    {
        return;
    }

    mediaPlayer->stop();
}

void VideoPlayerWindow::btnPauseClicked()
{
    if (mediaPlayer == nullptr)
    {
        return;
    }

    if (mediaPlayer->playbackState() == QMediaPlayer::PlaybackState::PausedState)
    {
        mediaPlayer->play();
    }
    else
    {
        mediaPlayer->pause();
    }
}

void VideoPlayerWindow::durationChanged(qint64 durationMilliseconds)
{
    mediaDurationMillis = durationMilliseconds;

    if (durationMilliseconds < 0)
    {
        ui->sliderPosition->setEnabled(false);
    }
    else
    {
        ui->sliderPosition->setMinimum(0);
        ui->sliderPosition->setValue(0);
        ui->sliderPosition->setMaximum(durationMilliseconds / 1000);
        ui->sliderPosition->setEnabled(true);
    }
}

void VideoPlayerWindow::positionChanged(qint64 position)
{
    showPosition(position, mediaDurationMillis);
    ui->sliderPosition->setValue(position / 1000);
}

void VideoPlayerWindow::seekableChanged(bool seekable)
{
    if (!seekable)
    {
        ui->sliderPosition->setEnabled(false);
        return;
    }

    // Seekable from now on, so adjust slider min/max.
    if (mediaDurationMillis > 0)
    {
        ui->sliderPosition->setMinimum(0);
        ui->sliderPosition->setMaximum(mediaDurationMillis / 1000);
        ui->sliderPosition->setEnabled(true);
    }
}

void VideoPlayerWindow::mediaErrorOccurred(QMediaPlayer::Error error, const QString &msg)
{
    if (error == QMediaPlayer::Error::NoError)
    {
        return;
    }
    if (this->isVisible())
    {
        QMessageBox::warning(this, "Fehler bei Videowiedergabe", "Ein Fehler ist aufgetreten: " + msg);
    }
    ui->statusbar->showMessage("Ein Fehler ist aufgetreten: " + msg);
}

void VideoPlayerWindow::actionAutoPlayVideoTriggered(bool checked)
{
    emit autoPlayChanged(checked);
}

void VideoPlayerWindow::actionLoopForeverTriggered(bool checked)
{
    if (mediaPlayer == nullptr)
    {
        return;
    }

    mediaPlayer->setLoops(checked
                              ? QMediaPlayer::Loops::Infinite
                              : QMediaPlayer::Loops::Once);

    emit loopForeverChanged(checked);
}

QString VideoPlayerWindow::supportedFormatsMessage()
{
    QString message = "Die Videowiedergabe unterstützt folgende Codecs:\n";
    QMediaFormat mf;
    const QList<QMediaFormat::VideoCodec> codecs = mf.supportedVideoCodecs(QMediaFormat::ConversionMode::Decode);
    for (const QMediaFormat::VideoCodec codec: codecs)
    {
        message += "\n" + QMediaFormat::videoCodecName(codec);
    }
    if (codecs.empty())
    {
        message += "\nKeine (Das ist wirklich schlecht.)";
    }

    message += "\n\nDie unterstützten Codecs können je nach System variieren.";
    return message;
}

void VideoPlayerWindow::actionSupportedFormatsTriggered()
{
    QMessageBox::about(this, "Unterstützte Videocodecs",
                       supportedFormatsMessage());
}

void VideoPlayerWindow::actionShowMetadataTriggered()
{
    if (mediaPlayer == nullptr)
    {
        return;
    }

    QString message = "Medienmetadaten:\n";
    const QMediaMetaData md = mediaPlayer->metaData();
    const QList<QMediaMetaData::Key> keys = md.keys();
    if (keys.isEmpty())
    {
        message += "\nKeine Metadaten vorhanden.";
    }
    for (const QMediaMetaData::Key key: keys)
    {
        message += "\n" + QMediaMetaData::metaDataKeyToString(key) + ": " + md.stringValue(key);
    }
    QMessageBox::about(this, "Metadaten", message);
}

void VideoPlayerWindow::showPosition(const qint64 currentPositionMs, const qint64 durationMs)
{
    if ((durationMs <= 0) || (currentPositionMs <= 0))
    {
        ui->lblPosition->setText("--:-- / --:--");
        return;
    }

    ui->lblPosition->setText(DurationUtils::durationToMinutesSeconds(currentPositionMs) + " / "
                             + DurationUtils::durationToMinutesSeconds(durationMs));
}

void VideoPlayerWindow::sliderPositionMoved(int position)
{
    if (mediaPlayer == nullptr)
    {
        return;
    }

    const qint64 new_position = qint64(position) * 1000;
    mediaPlayer->setPosition(new_position);
    showPosition(new_position, mediaDurationMillis);
}

void VideoPlayerWindow::sliderVolumeValueChanged(int value)
{
    if (audioOutput == nullptr)
    {
        return;
    }

// Convert from logarithmic scale to linear scale.
// QtAudio::convertVolume() was QAudio::convertVolume() before Qt 6.7, so
// we have to use different code depending on the Qt version used to
// compile this.
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    const qreal linearVolume = QtAudio::convertVolume(
        value / 100.0, QtAudio::LogarithmicVolumeScale,
        QtAudio::LinearVolumeScale);
#else
    const qreal linearVolume = QAudio::convertVolume(
        value / 100.0, QAudio::LogarithmicVolumeScale,
        QAudio::LinearVolumeScale);
#endif
    audioOutput->setVolume(linearVolume);

    ui->lblVolumeValue->setText(QString::number(value) + " %");
    emit videoVolumeChanged(value);
}
