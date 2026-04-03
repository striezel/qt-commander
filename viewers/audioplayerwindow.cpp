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

#include "audioplayerwindow.h"
#include "ui_audioplayerwindow.h"
#include "../mainwindow.h"

#include <algorithm> // for std::clamp()
#include <QAudio>
#include <QMediaMetaData>
#include <QMediaFormat>
#include <QMessageBox>

#include "util/durationutils.h"

AudioPlayerWindow::AudioPlayerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AudioPlayerWindow)
    , mediaPlayer(new QMediaPlayer(this))
    , audioOutput(new QAudioOutput())
    , mediaDurationMillis(-1)
{
    ui->setupUi(this);

    connect(ui->actionClose, &QAction::triggered, this, &AudioPlayerWindow::close);

    connect(ui->btnStart, &QPushButton::clicked, this, &AudioPlayerWindow::btnStartClicked);
    connect(ui->btnStop, &QPushButton::clicked, this, &AudioPlayerWindow::btnStopClicked);
    connect(ui->btnPause, &QPushButton::clicked, this, &AudioPlayerWindow::btnPauseClicked);

    // Using &QSlider::sliderMoved instead of &QSlider::valueChanged for position,
    // because otherwise the setting of a new value in code would trigger this
    // signal again, resulting in potentially endless loops.
    connect(ui->sliderPosition, &QSlider::sliderMoved, this, &AudioPlayerWindow::sliderPositionMoved);
    connect(ui->sliderVolume, &QSlider::valueChanged, this, &AudioPlayerWindow::sliderVolumeValueChanged);

    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &AudioPlayerWindow::durationChanged);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &AudioPlayerWindow::positionChanged);
    connect(mediaPlayer, &QMediaPlayer::seekableChanged, this, &AudioPlayerWindow::seekableChanged);

    connect(ui->actionAutoPlayAudio, &QAction::triggered, this, &AudioPlayerWindow::actionAutoPlayAudioTriggered);
    connect(ui->actionSupportedFormats, &QAction::triggered, this, &AudioPlayerWindow::actionSupportedFormatsTriggered);
    connect(ui->actionShowMetadata, &QAction::triggered, this, &AudioPlayerWindow::actionShowMetadataTriggered);

    // set initial volume to whatever the slider shows currently
    sliderVolumeValueChanged(ui->sliderVolume->value());

    if (parent != nullptr)
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent);
        connect(this, &AudioPlayerWindow::autoPlayChanged, castedParent, &MainWindow::audioPlayerAutoPlayChanged);
        connect(this, &AudioPlayerWindow::audioVolumeChanged, castedParent, &MainWindow::audioPlayerVolumeChanged);
    }
}

AudioPlayerWindow::~AudioPlayerWindow()
{
    // Stop any media which might still be playing.
    btnStopClicked();

    delete mediaPlayer;
    mediaPlayer = nullptr;

    if (parent() != nullptr)
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent());
        disconnect(this, &AudioPlayerWindow::autoPlayChanged, castedParent, &MainWindow::audioPlayerAutoPlayChanged);
        disconnect(this, &AudioPlayerWindow::audioVolumeChanged, castedParent, &MainWindow::audioPlayerVolumeChanged);
    }

    delete ui;
}

bool AudioPlayerWindow::loadAudioFile(const QString &path)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    if (mediaPlayer->isPlaying())
#else
    if (mediaPlayer->playbackState() == QMediaPlayer::PlaybackState::PlayingState)
#endif
    {
        mediaPlayer->stop();
    }
    mediaPlayer->setSource(QUrl::fromLocalFile(path));

    mediaPlayer->setAudioOutput(audioOutput);

    setWindowTitle("Audiowiedergabe - " + path);

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

void AudioPlayerWindow::setAutoPlay(const bool autoPlay)
{
    ui->actionAutoPlayAudio->setChecked(autoPlay);
}

void AudioPlayerWindow::setVolume(const int volume)
{
    ui->sliderVolume->setValue(std::clamp(volume, 0, 100));
}

void AudioPlayerWindow::closeEvent(QCloseEvent *event)
{
    // ensure deletion
    this->deleteLater();

    // do the usual event handling inherited from base class
    this->QMainWindow::closeEvent(event);
}

void AudioPlayerWindow::showEvent(QShowEvent *event)
{
    if (ui->actionAutoPlayAudio->isChecked())
    {
        btnStartClicked();
    }
}

void AudioPlayerWindow::btnStartClicked()
{
    if (mediaPlayer == nullptr)
    {
        return;
    }

    mediaPlayer->play();
}

void AudioPlayerWindow::btnStopClicked()
{
    if (mediaPlayer == nullptr)
    {
        return;
    }

    mediaPlayer->stop();
}

void AudioPlayerWindow::btnPauseClicked()
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

void AudioPlayerWindow::sliderPositionMoved(int position)
{
    if (mediaPlayer == nullptr)
    {
        return;
    }

    const qint64 new_position = qint64(position) * 1000;
    mediaPlayer->setPosition(new_position);
    showPosition(new_position, mediaDurationMillis);
}

void AudioPlayerWindow::sliderVolumeValueChanged(int value)
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
    emit audioVolumeChanged(value);
}

void AudioPlayerWindow::durationChanged(qint64 durationMilliseconds)
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

void AudioPlayerWindow::positionChanged(qint64 position)
{
    showPosition(position, mediaDurationMillis);
    ui->sliderPosition->setValue(position / 1000);
}

void AudioPlayerWindow::seekableChanged(bool seekable)
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

void AudioPlayerWindow::actionAutoPlayAudioTriggered(bool checked)
{
    emit autoPlayChanged(checked);
}

QString AudioPlayerWindow::supportedFormatsMessage()
{
    QString message = "Die Audiowiedergabe unterstützt folgende Audiocodecs:\n";
    QMediaFormat mf;
    const QList<QMediaFormat::AudioCodec> codecs = mf.supportedAudioCodecs(QMediaFormat::ConversionMode::Decode);
    for (const QMediaFormat::AudioCodec codec: codecs)
    {
        message += "\n" + QMediaFormat::audioCodecName(codec);
    }
    if (codecs.empty())
    {
        message += "\nKeine (Das ist wirklich schlecht.)";
    }

    message += "\n\nDie unterstützten Codecs können je nach System variieren.";
    return message;
}

void AudioPlayerWindow::actionSupportedFormatsTriggered()
{
    QMessageBox::about(this, "Unterstützte Formate und Audiocodecs",
                       supportedFormatsMessage());
}

void AudioPlayerWindow::actionShowMetadataTriggered()
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

void AudioPlayerWindow::showPosition(const qint64 currentPositionMs, const qint64 durationMs)
{
    if ((durationMs <= 0) || (currentPositionMs <= 0))
    {
        ui->lblPosition->setText("--:-- / --:--");
        return;
    }

    ui->lblPosition->setText(DurationUtils::durationToMinutesSeconds(currentPositionMs) + " / "
                             + DurationUtils::durationToMinutesSeconds(durationMs));
}
