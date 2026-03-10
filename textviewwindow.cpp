#include "textviewwindow.h"
#include "ui_textviewwindow.h"

#include "mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QFontDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QScrollBar>
#include <QTextStream>

TextViewWindow::TextViewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TextViewWindow)
{
    ui->setupUi(this);

    connect(ui->actionPrint, &QAction::triggered, this, &TextViewWindow::actionPrintTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &TextViewWindow::close);

    connect(ui->actionChangeFont, &QAction::triggered, this, &TextViewWindow::actionChangeFontTriggered);

    setMonospacedFont();

    if (parent != nullptr)
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent);
        connect(this, &TextViewWindow::textViewerFontChanged, castedParent, &MainWindow::textViewerFontChanged);
    }
}

TextViewWindow::~TextViewWindow()
{
    if (parent() != nullptr)
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent());
        disconnect(this, &TextViewWindow::textViewerFontChanged, castedParent, &MainWindow::textViewerFontChanged);
    }

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
        // read up to one megabyte of data in one go
        line = stream.read(1024 * 1024);
        ui->plainTextEdit->insertPlainText(line);
    }
    ui->plainTextEdit->setDocumentTitle(file.fileName());
    file.close();

    this->setWindowTitle("Textbetrachter - " + path);

    return true;
}

void TextViewWindow::setFont(const QFont &font)
{
    ui->plainTextEdit->setFont(font);
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

void TextViewWindow::actionPrintTriggered()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() != QPrintDialog::Accepted)
    {
        return;
    }

    printer.setDocName(ui->plainTextEdit->documentTitle());
    ui->plainTextEdit->print(&printer);
}

void TextViewWindow::actionChangeFontTriggered()
{
    bool ok = false;
    const QFont new_font = QFontDialog::getFont(&ok, ui->plainTextEdit->font(), this);
    if (!ok)
    {
        return;
    }

    ui->plainTextEdit->setFont(new_font);
    emit textViewerFontChanged(new_font);
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
