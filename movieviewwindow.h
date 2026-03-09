#ifndef MOVIEVIEWWINDOW_H
#define MOVIEVIEWWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QMovie>

namespace Ui {
class MovieViewWindow;
}

class MovieViewWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MovieViewWindow(QWidget *parent = nullptr);
    ~MovieViewWindow();

    /// Loads the file specified by path as movie file.
    ///
    /// Returns true, if movie was successfully loaded.
    /// Returns false otherwise.
    bool loadMovieFile(const QString& path);

    /// Sets whether movies should start automatically when the viewer is shown.
    void setAutoStartVideos(const bool autoStart);

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
    Ui::MovieViewWindow *ui;

    QMovie* movie;
};

#endif // MOVIEVIEWWINDOW_H
