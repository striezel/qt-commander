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
protected:
    void closeEvent(QCloseEvent* event) override;
private slots:
    void btnStartClicked();
    void btnStopClicked();
    void btnPauseClicked();

    void actionSupportedFileTypesTriggered();

private:
    Ui::MovieViewWindow *ui;

    QMovie* movie;
};

#endif // MOVIEVIEWWINDOW_H
