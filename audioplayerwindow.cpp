#include "audioplayerwindow.h"
#include "ui_audioplayerwindow.h"

#include <QMediaMetaData>

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

    // set initial volume to whatever the slider shows currently
    sliderVolumeValueChanged(ui->sliderVolume->value());
}

AudioPlayerWindow::~AudioPlayerWindow()
{
    // Stop any media which might still be playing.
    btnStopClicked();

    delete mediaPlayer;
    mediaPlayer = nullptr;

    delete ui;
}

bool AudioPlayerWindow::loadAudioFile(const QString &path)
{
    if (mediaPlayer->isPlaying())
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

void AudioPlayerWindow::closeEvent(QCloseEvent *event)
{
    // ensure deletion
    this->deleteLater();

    // do the usual event handling inherited from base class
    this->QMainWindow::closeEvent(event);
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

    mediaPlayer->pause();
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
    const qreal linearVolume = QtAudio::convertVolume(
        value / 100.0, QtAudio::LogarithmicVolumeScale,
        QtAudio::LinearVolumeScale);
    audioOutput->setVolume(linearVolume);

    ui->lblVolumeValue->setText(QString::number(value) + " %");
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

QString AudioPlayerWindow::durationToMinutesSeconds(const qint64 durationMs)
{
    if (durationMs < 0)
    {
        return "--:--";
    }

    qint64 seconds = durationMs / 1000; // rounded down
    const qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    return QString::number(minutes) + ":"
           + (seconds < 10 ? "0" : "") + QString::number(seconds);
}

void AudioPlayerWindow::showPosition(const qint64 currentPositionMs, const qint64 durationMs)
{
    if ((durationMs <= 0) || (currentPositionMs <= 0))
    {
        ui->lblPosition->setText("--:-- / --:--");
        return;
    }

    ui->lblPosition->setText(durationToMinutesSeconds(currentPositionMs) + " / "
                             + durationToMinutesSeconds(durationMs));
}
