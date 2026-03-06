#include "textviewwindow.h"
#include "ui_textviewwindow.h"

#include <QDebug>
#include <QFile>
#include <QScrollBar>
#include <QTextStream>

TextViewWindow::TextViewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TextViewWindow)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &TextViewWindow::close);

    setMonospacedFont();
}

TextViewWindow::~TextViewWindow()
{
    delete ui;
}

bool TextViewWindow::loadTextFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text))
    {
        return false;
    }

    ui->plainTextEdit->clear();
    QTextStream stream(&file);
    QString line;
    while (!stream.atEnd())
    {
        line = stream.readLine();
        ui->plainTextEdit->appendPlainText(line);
    }
    ui->plainTextEdit->setDocumentTitle(file.fileName());
    file.close();

    this->setWindowTitle("Textbetrachter - " + path);

    return true;
}

void TextViewWindow::closeEvent(QCloseEvent *event)
{
    // ensure deletion
    this->deleteLater();

    // do the usual event handling inherited from base class
    this->QMainWindow::closeEvent(event);
}

void TextViewWindow::showEvent(QShowEvent *event)
{
    scrollToTop();
}

void TextViewWindow::scrollToTop()
{
    QScrollBar* scrollBar = ui->plainTextEdit->verticalScrollBar();
    if (scrollBar == nullptr)
    {
        return;
    }

    scrollBar->setValue(scrollBar->minimum());
}

void TextViewWindow::setMonospacedFont()
{
    QFont font = QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont);
    qDebug() << "Setting font to " << font;
    ui->plainTextEdit->setFont(font);
}
