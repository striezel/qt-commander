/*
 -------------------------------------------------------------------------------
    This file is part of the Qt Commander file manager.
    Copyright (C) 2026  Dirk Stolle

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------------
*/

#include "textviewwindow.h"
#include "ui_textviewwindow.h"

#include "mainwindow.h"

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
    // The read loop checks for stream.atEnd() at the end and not at the start
    // of the loop, because for some files on virutal file systems like /proc on
    // Linux stream.atEnd() is true, even if there is data to read. So we try at
    // least one read operation before leaving the loop. Not ideal but gets the
    // job done in most cases. With the check at the top of the loop the text
    // viewer would just display an empty plain text edit component for most
    // files on the /proc file system.
    do
    {
        // read up to one megabyte of data in one go
        line = stream.read(1024 * 1024);
        if (!line.isNull())
        {
            ui->plainTextEdit->insertPlainText(line);
        }
    }
    while (!stream.atEnd());
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
    ui->plainTextEdit->setFont(font);
}
