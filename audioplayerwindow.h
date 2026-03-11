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
protected:
    void closeEvent(QCloseEvent* event) override;
private slots:
    void btnStartClicked();
    void btnStopClicked();
    void btnPauseClicked();

    void sliderVolumeValueChanged(int value);
private:
    Ui::AudioPlayerWindow *ui;

    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;
};

#endif // AUDIOPLAYERWINDOW_H
