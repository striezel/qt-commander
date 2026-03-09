#include "movieviewwindow.h"
#include "ui_movieviewwindow.h"

#include <QMessageBox>

MovieViewWindow::MovieViewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MovieViewWindow)
    , movie(nullptr)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &MovieViewWindow::close);
    connect(ui->actionSupportedFileTypes, &QAction::triggered,
            this, &MovieViewWindow::actionSupportedFileTypesTriggered);

    connect(ui->btnStart, &QPushButton::clicked, this, &MovieViewWindow::btnStartClicked);
    connect(ui->btnStop, &QPushButton::clicked, this, &MovieViewWindow::btnStopClicked);
    connect(ui->btnPause, &QPushButton::clicked, this, &MovieViewWindow::btnPauseClicked);
}

MovieViewWindow::~MovieViewWindow()
{
    if (movie != nullptr)
    {
        movie->stop();
        delete movie;
        movie = nullptr;
    }

    delete ui;
}

bool MovieViewWindow::loadMovieFile(const QString &path)
{
    QMovie* new_movie = new QMovie(path);
    if (!new_movie->isValid())
    {
        return false;
    }

    ui->label->setMovie(new_movie);

    setWindowTitle("Videobetrachter - " + path);
    if (movie != nullptr)
    {
        if (movie->state() == QMovie::MovieState::Running)
        {
            movie->stop();
        }
        delete movie;
        movie = nullptr;
    }
    movie = new_movie;

    return true;
}

void MovieViewWindow::closeEvent(QCloseEvent *event)
{
    // ensure deletion
    this->deleteLater();

    // do the usual event handling inherited from base class
    this->QMainWindow::closeEvent(event);
}

void MovieViewWindow::btnStartClicked()
{
    if (movie == nullptr)
    {
        return;
    }

    movie->start();
}

void MovieViewWindow::btnStopClicked()
{
    if (movie == nullptr)
    {
        return;
    }

    movie->stop();
}

void MovieViewWindow::btnPauseClicked()
{
    if (movie == nullptr)
    {
        return;
    }

    movie->setPaused(true);
}

void MovieViewWindow::actionSupportedFileTypesTriggered()
{
    QString message = "Der Videobetrachter unterstützt folgende MIME-Typen:\n";
    for (const QByteArray& element: QMovie::supportedFormats())
    {
        message = message + "\n" + element;
    }
    QMessageBox::about(this, "Unterstützte Dateiformate", message);
}
