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

#ifndef TEXTVIEWWINDOW_H
#define TEXTVIEWWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QCloseEvent>
#include <QFont>
#include <QString>
#include <QSyntaxHighlighter>
#include "hl/themes/theme.h"

namespace Ui {
class TextViewWindow;
}

/// A window that can be used to view text files.
class TextViewWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextViewWindow(QWidget* parent = nullptr);
    ~TextViewWindow();

    /// Loads the file specified by path as text file.
    ///
    /// Returns true, if text was successfully loaded.
    /// Returns false otherwise.
    bool loadTextFile(const QString& path);

    /// Sets the font for the text widget.
    void setFont(const QFont& font);

signals:
    void textViewerFontChanged(const QFont& new_font);

protected:
    void closeEvent(QCloseEvent* event) override;

    void showEvent(QShowEvent* event) override;

private slots:
    void actionPrintTriggered();
    void actionChangeFontTriggered();

    void actionLanguageChangeTriggered();
    void actionLanguageNoneTriggered();

    void actionStyleChangeTriggered(bool checked = false);

private:
    Ui::TextViewWindow *ui;

    QActionGroup* actionGroupLanguages;
    QActionGroup* actionGroupStyles;
    QSyntaxHighlighter* hl;

    /// Scrolls the text widget back to the top.
    void scrollToTop();

    /// Set the system's recommended fixed-size font for the text widget.
    void setMonospacedFont();

    /// Remove existing syntax highlighting.
    void removeHighlighter();

    Theme* getSelectedTheme() const;

    QSyntaxHighlighter* getSelectedHighlighter(const Theme& theme) const;

    void updateWithNewTheme();

    // Creates action groups for the syntax highlighting menu.
    void createActionGroups();
};

#endif // TEXTVIEWWINDOW_H
