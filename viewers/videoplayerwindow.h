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

#ifndef VIDEOPLAYERWINDOW_H
#define VIDEOPLAYERWINDOW_H

#include <QAudioOutput>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QString>

namespace Ui {
class VideoPlayerWindow;
}

class VideoPlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoPlayerWindow(QWidget *parent = nullptr);
    ~VideoPlayerWindow();

    /// Loads the file specified by path as video file.
    ///
    /// Returns true, if the video file was successfully loaded.
    /// Returns false otherwise.
    bool loadVideoFile(const QString& path);

    /// Sets whether files should start playing automatically when the viewer is shown.
    void setAutoPlay(const bool autoPlay);

    /// Sets whether files should playing in loop indefinitely.
    void setLoopForever(const bool loopForever);

    /// Sets the volume for video playback. Must be in range [0;100].
    void setVolume(const int volume);

    /// Gets a message describing the supported video formats.
    static QString supportedFormatsMessage();
signals:
    void autoPlayChanged(const bool autoPlay);
    void loopForeverChanged(const bool loopForever);
    void videoVolumeChanged(const int volume);
protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
private slots:
    void btnStartClicked();
    void btnStopClicked();
    void btnPauseClicked();

    void sliderPositionMoved(int position);
    void sliderVolumeValueChanged(int value);

    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void seekableChanged(bool seekable);
    void mediaErrorOccurred(QMediaPlayer::Error error, const QString& msg);

    void actionAutoPlayVideoTriggered(bool checked = false);
    void actionLoopForeverTriggered(bool checked = false);
    void actionSupportedFormatsTriggered();
    void actionShowMetadataTriggered();
private:
    Ui::VideoPlayerWindow *ui;

    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;

    // duration of the media in milliseconds, or -1 if unknown
    qint64 mediaDurationMillis;

    void showPosition(const qint64 currentPositionMs, const qint64 durationMs);
};

#endif // VIDEOPLAYERWINDOW_H
