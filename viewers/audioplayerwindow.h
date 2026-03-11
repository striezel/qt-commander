#ifndef AUDIOPLAYERWINDOW_H
#define AUDIOPLAYERWINDOW_H

#include <QAudioOutput>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMediaPlayer>

namespace Ui {
class AudioPlayerWindow;
}

class AudioPlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AudioPlayerWindow(QWidget *parent = nullptr);
    ~AudioPlayerWindow();

    /// Loads the file specified by path as audio file.
    ///
    /// Returns true, if the audio file was successfully loaded.
    /// Returns false otherwise.
    bool loadAudioFile(const QString& path);

    /// Sets whether files should start playing automatically when the viewer is shown.
    void setAutoPlay(const bool autoPlay);

    /// Sets the volume for audio playback. Must be in range [0;100].
    void setVolume(const int volume);
signals:
    void autoPlayChanged(const bool autoPlay);
    void audioVolumeChanged(const int volume);
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

    void actionAutoPlayAudioTriggered(bool checked = false);
private:
    Ui::AudioPlayerWindow *ui;

    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;

    // duration of the media in milliseconds, or -1 if unknown
    qint64 mediaDurationMillis;

    void showPosition(const qint64 currentPositionMs, const qint64 durationMs);
    static QString durationToMinutesSeconds(const qint64 durationMs);
};

#endif // AUDIOPLAYERWINDOW_H
