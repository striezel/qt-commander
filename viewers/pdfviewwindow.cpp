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

#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
#include <QPdfPageNavigator>
#else
#include <QPdfPageNavigation>
#endif

PdfViewWindow::PdfViewWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PdfViewWindow)
    , document(new QPdfDocument(this))
    , pageSpinBox(createPageSpinBox())
    , jumpOnSpinBoxChange(true)
{
    ui->setupUi(this);

    connect(ui->actionClose, &QAction::triggered, this, &PdfViewWindow::close);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
    QPdfPageNavigator* nav = ui->pdfView->pageNavigator();
    connect(nav, &QPdfPageNavigator::currentPageChanged, this, &PdfViewWindow::currentPageChanged);
#else
    QPdfPageNavigation* nav = ui->pdfView->pageNavigation();
    connect(nav, &QPdfPageNavigation::currentPageChanged, this, &PdfViewWindow::currentPageChanged);
#endif
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
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
    const QPdfDocument::Error error = document->load(path);
#else
    const QPdfDocument::DocumentError error = document->load(path);
#endif
    updatePrevNextStatus();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
    if (error == QPdfDocument::Error::None)
#else
    if (error == QPdfDocument::DocumentError::NoError)
#endif
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
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
    return error == QPdfDocument::Error::None;
#else
    return error == QPdfDocument::DocumentError::NoError;
#endif
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
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
    QPdfPageNavigator* nav = ui->pdfView->pageNavigator();
    nav->jump(nav->currentPage() - 1, {}, nav->currentZoom());
#else
    ui->pdfView->pageNavigation()->goToPreviousPage();
#endif
    updatePrevNextStatus();
}

void PdfViewWindow::actionNextPageTriggered()
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
    QPdfPageNavigator* nav = ui->pdfView->pageNavigator();
    nav->jump(nav->currentPage() + 1, {}, nav->currentZoom());
#else
    ui->pdfView->pageNavigation()->goToNextPage();
#endif
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

#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
    QPdfPageNavigator* nav = ui->pdfView->pageNavigator();
    nav->jump(value - 1, {}, nav->currentZoom());
#else
    QPdfPageNavigation* nav = ui->pdfView->pageNavigation();
    nav->setCurrentPage(value - 1);
#endif
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
#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
    QPdfPageNavigator* nav = ui->pdfView->pageNavigator();
#else
    QPdfPageNavigation* nav = ui->pdfView->pageNavigation();
#endif
    // The value from currentPage() is zero-based, so first page is 0 and not 1.
    ui->actionPreviousPage->setEnabled(nav->currentPage() > 0);
    // The value from currentPage() is zero-based, so the last page is
    // pageCount() - 1.
    ui->actionNextPage->setEnabled(nav->currentPage() < document->pageCount() - 1);
}
