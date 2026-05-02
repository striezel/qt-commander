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

#ifndef PDFVIEWWINDOW_H
#define PDFVIEWWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QPdfDocument>
#include <QSpinBox>

namespace Ui {
class PdfViewWindow;
}

class PdfViewWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PdfViewWindow(QWidget *parent = nullptr);
    ~PdfViewWindow();

    /// Loads the file specified by path as PDF file.
    ///
    /// Returns true, if PDF was successfully loaded.
    /// Returns false otherwise.
    bool loadPdfFile(const QString& path);
protected:
    void closeEvent(QCloseEvent* event) override;
private slots:
    void actionPreviousPageTriggered();
    void actionNextPageTriggered();

    void actionZoomOutTriggered();
    void actionZoomInTriggered();
    void actionZoomToFitTriggered();

    void currentPageChanged(int page);
    void pageSpinBoxValueChanged(int value);
private:
    Ui::PdfViewWindow *ui;

    QPdfDocument* document;
    // Qt's QPdfPageSelector seems to be buggy, allowing one more page than the
    // PDF document has, so we have to use our own spin box instead.
    QSpinBox* pageSpinBox;
    /// whether a spin box value change causes a page jump in the document
    bool jumpOnSpinBoxChange;

    QSpinBox* createPageSpinBox();
    void updatePrevNextStatus();
};

#endif // PDFVIEWWINDOW_H
