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

#include "pdfviewwindow.h"
#include "ui_pdfviewwindow.h"

#include <QPdfPageNavigator>

PdfViewWindow::PdfViewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PdfViewWindow)
    , document(new QPdfDocument(this))
    , pageSpinBox(createPageSpinBox())
    , jumpOnSpinBoxChange(true)
{
    ui->setupUi(this);

    connect(ui->actionClose, &QAction::triggered, this, &PdfViewWindow::close);

    QPdfPageNavigator* nav = ui->pdfView->pageNavigator();
    connect(nav, &QPdfPageNavigator::currentPageChanged, this, &PdfViewWindow::currentPageChanged);
    connect(ui->actionPreviousPage, &QAction::triggered, this, &PdfViewWindow::actionPreviousPageTriggered);
    connect(ui->actionNextPage, &QAction::triggered, this, &PdfViewWindow::actionNextPageTriggered);
    connect(ui->actionZoomOut, &QAction::triggered, this, &PdfViewWindow::actionZoomOutTriggered);
    connect(ui->actionZoomIn, &QAction::triggered, this, &PdfViewWindow::actionZoomInTriggered);

    ui->toolBar->insertWidget(ui->actionNextPage, pageSpinBox);
    connect(pageSpinBox, &QSpinBox::valueChanged, this, &PdfViewWindow::pageSpinBoxValueChanged);

    ui->pdfView->setPageMode(QPdfView::PageMode::MultiPage);
    ui->pdfView->setDocument(document);
}

PdfViewWindow::~PdfViewWindow()
{
    delete ui;

    document->close();
}

bool PdfViewWindow::loadPdfFile(const QString &path)
{
    const QPdfDocument::Error error = document->load(path);
    updatePrevNextStatus();
    if (error == QPdfDocument::Error::None)
    {
        setWindowTitle("PDF-Betrachter - " + path);
        const int pageCount = document->pageCount();
        statusBar()->showMessage(
            "PDF geladen, " + QString::number(pageCount)
            + " Seite(n)");
        pageSpinBox->setValue(1);
        pageSpinBox->setMaximum(pageCount);
        pageSpinBox->setSuffix(" / " + QString::number(pageCount));
    }
    return error == QPdfDocument::Error::None;
}

void PdfViewWindow::closeEvent(QCloseEvent *event)
{
    // do the usual event handling inherited from base class
    this->QMainWindow::closeEvent(event);

    // ensure deletion
    this->deleteLater();
}

void PdfViewWindow::actionPreviousPageTriggered()
{
    QPdfPageNavigator* nav = ui->pdfView->pageNavigator();
    nav->jump(nav->currentPage() - 1, {}, nav->currentZoom());
    updatePrevNextStatus();
}

void PdfViewWindow::actionNextPageTriggered()
{
    QPdfPageNavigator* nav = ui->pdfView->pageNavigator();
    nav->jump(nav->currentPage() + 1, {}, nav->currentZoom());
    updatePrevNextStatus();
}

void PdfViewWindow::actionZoomOutTriggered()
{
    ui->pdfView->setZoomFactor(ui->pdfView->zoomFactor() / 1.5);
}

void PdfViewWindow::actionZoomInTriggered()
{
    ui->pdfView->setZoomFactor(ui->pdfView->zoomFactor() * 1.5);
}

void PdfViewWindow::currentPageChanged(int page)
{
    updatePrevNextStatus();
    jumpOnSpinBoxChange = false;
    pageSpinBox->setValue(page + 1);
    jumpOnSpinBoxChange = true;
}

void PdfViewWindow::pageSpinBoxValueChanged(int value)
{
    if (!jumpOnSpinBoxChange)
    {
        return;
    }

    QPdfPageNavigator* nav = ui->pdfView->pageNavigator();
    nav->jump(value - 1, {}, nav->currentZoom());
}

QSpinBox *PdfViewWindow::createPageSpinBox()
{
    QSpinBox* spinBox = new QSpinBox(this);
    spinBox->setValue(1);
    spinBox->setMinimum(1);
    spinBox->setMaximum(1);
    spinBox->setPrefix("Seite ");
    spinBox->setSuffix(" / 1");
    return spinBox;
}

void PdfViewWindow::updatePrevNextStatus()
{
    QPdfPageNavigator* nav = ui->pdfView->pageNavigator();
    // The value from currentPage() is zero-based, so first page is 0 and not 1.
    ui->actionPreviousPage->setEnabled(nav->currentPage() > 0);
    // The value from currentPage() is zero-based, so the last page is
    // pageCount() - 1.
    ui->actionNextPage->setEnabled(nav->currentPage() < document->pageCount() - 1);
}
