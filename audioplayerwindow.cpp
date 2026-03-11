#include "audioplayerwindow.h"
#include "ui_audioplayerwindow.h"

AudioPlayerWindow::AudioPlayerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AudioPlayerWindow)
    , mediaPlayer(new QMediaPlayer(this))
    , audioOutput(new QAudioOutput())
{
    ui->setupUi(this);

    connect(ui->actionClose, &QAction::triggered, this, &AudioPlayerWindow::close);

    connect(ui->btnStart, &QPushButton::clicked, this, &AudioPlayerWindow::btnStartClicked);
    connect(ui->btnStop, &QPushButton::clicked, this, &AudioPlayerWindow::btnStopClicked);
    connect(ui->btnPause, &QPushButton::clicked, this, &AudioPlayerWindow::btnPauseClicked);

    connect(ui->sliderVolume, &QSlider::valueChanged, this, &AudioPlayerWindow::sliderVolumeValueChanged);

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
