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

    /// Sets whether movies should start automatically when the viewer is shown.
    void setAutoStartVideos(const bool autoStart);

    /// Gets a message describing the supported file formats.
    static QString supportedFormatsMessage();
signals:
    void autoStartChanged(const bool autoStart);
protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
private slots:
    void btnStartClicked();
    void btnStopClicked();
    void btnPauseClicked();

    void actionAutoStartVideosTriggered(bool checked = false);
    void actionSupportedFileTypesTriggered();

private:
    Ui::AnimationViewWindow *ui;

    QMovie* movie;
};

#endif // ANIMATIONVIEWWINDOW_H
