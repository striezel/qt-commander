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
#include "hl/cpphighlighter.h"
#include "hl/csharphighlighter.h"
#include "hl/javahighlighter.h"
#include "hl/pythonhighlighter.h"
#include "hl/rusthighlighter.h"
#include "hl/shellhighlighter.h"
#include "hl/sqlhighlighter.h"
#include "hl/themes/defaultthemedark.h"
#include "hl/themes/defaultthemelight.h"

TextViewWindow::TextViewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TextViewWindow)
    , documentPath(QStringLiteral(""))
    , actionGroupLanguages(nullptr)
    , actionGroupStyles(nullptr)
    , hl(nullptr)
{
    ui->setupUi(this);

    connect(ui->actionPrint, &QAction::triggered, this, &TextViewWindow::actionPrintTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &TextViewWindow::close);

    connect(ui->actionChangeFont, &QAction::triggered, this, &TextViewWindow::actionChangeFontTriggered);

    connect(ui->actionAutoSelectLanguage, &QAction::triggered, this, &TextViewWindow::actionAutoSelectLanguageTriggered);
    connect(ui->actionLanguageCpp, &QAction::triggered, this, &TextViewWindow::actionLanguageChangeTriggered);
    connect(ui->actionLanguageCSharp, &QAction::triggered, this, &TextViewWindow::actionLanguageChangeTriggered);
    connect(ui->actionLanguageJava, &QAction::triggered, this, &TextViewWindow::actionLanguageChangeTriggered);
    connect(ui->actionLanguagePython, &QAction::triggered, this, &TextViewWindow::actionLanguageChangeTriggered);
    connect(ui->actionLanguageRust, &QAction::triggered, this, &TextViewWindow::actionLanguageChangeTriggered);
    connect(ui->actionLanguageShell, &QAction::triggered, this, &TextViewWindow::actionLanguageChangeTriggered);
    connect(ui->actionLanguageSQL, &QAction::triggered, this, &TextViewWindow::actionLanguageChangeTriggered);
    connect(ui->actionLanguageNone, &QAction::triggered, this, &TextViewWindow::actionLanguageNoneTriggered);

    connect(ui->actionStyleAyuDark, &QAction::triggered, this, &TextViewWindow::actionStyleChangeTriggered);
    connect(ui->actionStyleAyuLight, &QAction::triggered, this, &TextViewWindow::actionStyleChangeTriggered);

    createActionGroups();

    setMonospacedFont();

    if (parent != nullptr)
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent);
        connect(this, &TextViewWindow::textViewerFontChanged, castedParent, &MainWindow::textViewerFontChanged);
        connect(this, &TextViewWindow::textViewerAutoSelectChanged, castedParent, &MainWindow::textViewerAutoSelectChanged);
        connect(this, &TextViewWindow::highlightingThemeChanged, castedParent, &MainWindow::textViewerHightlightingThemeChanged);
    }
}

TextViewWindow::~TextViewWindow()
{
    if (parent() != nullptr)
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent());
        disconnect(this, &TextViewWindow::textViewerFontChanged, castedParent, &MainWindow::textViewerFontChanged);
        disconnect(this, &TextViewWindow::textViewerAutoSelectChanged, castedParent, &MainWindow::textViewerAutoSelectChanged);
        disconnect(this, &TextViewWindow::highlightingThemeChanged, castedParent, &MainWindow::textViewerHightlightingThemeChanged);
    }

    removeHighlighter();

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
    // of the loop, because for some files on virtual file systems like /proc on
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
    documentPath = path;

    return true;
}

void TextViewWindow::setFont(const QFont &font)
{
    ui->plainTextEdit->setFont(font);
}

void TextViewWindow::setAutoSelectLanguage(const bool autoSelect)
{
    ui->actionAutoSelectLanguage->setChecked(autoSelect);

    if (autoSelect)
    {
        autoSelectHighlighting();
    }
}

void TextViewWindow::setHightlighterTheme(const ThemeId theme)
{
    if (theme == ThemeId::DefaultDark)
    {
        ui->actionStyleAyuDark->setChecked(true);
    }
    else
    {
        ui->actionStyleAyuLight->setChecked(true);
    }
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
    Q_UNUSED(event);

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

void TextViewWindow::actionAutoSelectLanguageTriggered(bool checked)
{
    emit textViewerAutoSelectChanged(checked);

    if (checked)
    {
        autoSelectHighlighting();
    }
}

void TextViewWindow::actionLanguageChangeTriggered()
{
    removeHighlighter();
    const std::unique_ptr<Theme> theme = getSelectedTheme();
    hl = getSelectedHighlighter(*theme);
}

void TextViewWindow::actionLanguageNoneTriggered()
{
    removeHighlighter();
}

void TextViewWindow::actionStyleChangeTriggered(bool checked)
{
    if (!checked)
    {
        return;
    }

    updateWithNewTheme();
    emit highlightingThemeChanged(getSelectedTheme()->id());
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

void TextViewWindow::removeHighlighter()
{
    if (hl != nullptr)
    {
        hl->setDocument(nullptr);
        delete hl;
        hl = nullptr;
    }
}

std::unique_ptr<Theme> TextViewWindow::getSelectedTheme() const
{
    if (ui->actionStyleAyuDark->isChecked())
    {
        return std::unique_ptr<DefaultThemeDark>(new DefaultThemeDark());
    }

    // Otherwise the light theme is checked.
    return std::unique_ptr<DefaultThemeLight>(new DefaultThemeLight());
}

QSyntaxHighlighter *TextViewWindow::getSelectedHighlighter(const Theme &theme) const
{
    if (ui->actionLanguageCpp->isChecked())
    {
        return new CppHighlighter(theme, ui->plainTextEdit->document());
    }
    else if (ui->actionLanguageCSharp->isChecked())
    {
        return new CSharpHighlighter(theme, ui->plainTextEdit->document());
    }
    else if (ui->actionLanguageJava->isChecked())
    {
        return new JavaHighlighter(theme, ui->plainTextEdit->document());
    }
    else if (ui->actionLanguagePython->isChecked())
    {
        return new PythonHighlighter(theme, ui->plainTextEdit->document());
    }
    else if (ui->actionLanguageRust->isChecked())
    {
        return new RustHighlighter(theme, ui->plainTextEdit->document());
    }
    else if (ui->actionLanguageShell->isChecked())
    {
        return new ShellHighlighter(theme, ui->plainTextEdit->document());
    }
    else if (ui->actionLanguageSQL->isChecked())
    {
        return new SqlHighlighter(theme, ui->plainTextEdit->document());
    }

    // no selection
    return nullptr;
}

void TextViewWindow::autoSelectHighlighting()
{
    const QString suffix = QFileInfo(documentPath).suffix().toLower();

    if ((suffix == "cpp") || (suffix == "cxx") || (suffix == "h")
        || (suffix == "hpp")  || (suffix == "hxx"))
    {
        ui->actionLanguageCpp->setChecked(true);
    }
    else if (suffix == "cs")
    {
        ui->actionLanguageCSharp->setChecked(true);
    }
    else if (suffix == "java")
    {
        ui->actionLanguageJava->setChecked(true);
    }
    else if (suffix == "py")
    {
        ui->actionLanguagePython->setChecked(true);
    }
    else if (suffix == "rs")
    {
        ui->actionLanguageRust->setChecked(true);
    }
    else if (suffix == "sh")
    {
        ui->actionLanguageShell->setChecked(true);
    }
    else if (suffix == "sql")
    {
        ui->actionLanguageSQL->setChecked(true);
    }
    else
    {
        ui->actionLanguageNone->setChecked(true);
    }
    actionLanguageChangeTriggered();
}

void TextViewWindow::updateWithNewTheme()
{
    if (ui->actionLanguageNone->isChecked())
    {
        return;
    }

    removeHighlighter();
    const std::unique_ptr<Theme> theme = getSelectedTheme();
    hl = getSelectedHighlighter(*theme);
}

void TextViewWindow::createActionGroups()
{
    actionGroupLanguages = new QActionGroup(this);
    actionGroupLanguages->addAction(ui->actionLanguageNone);
    actionGroupLanguages->addAction(ui->actionLanguageCpp);
    actionGroupLanguages->addAction(ui->actionLanguageCSharp);
    actionGroupLanguages->addAction(ui->actionLanguageJava);
    actionGroupLanguages->addAction(ui->actionLanguagePython);
    actionGroupLanguages->addAction(ui->actionLanguageRust);
    actionGroupLanguages->addAction(ui->actionLanguageShell);
    actionGroupLanguages->addAction(ui->actionLanguageSQL);

    actionGroupStyles = new QActionGroup(this);
    actionGroupStyles->addAction(ui->actionStyleAyuDark);
    actionGroupStyles->addAction(ui->actionStyleAyuLight);
}
