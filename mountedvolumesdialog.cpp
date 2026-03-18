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

#include "mountedvolumesdialog.h"
#include "mainwindow.h"
#include "ui_mountedvolumesdialog.h"

#include <QPushButton>
#include <QStorageInfo>

MountedVolumesDialog::MountedVolumesDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MountedVolumesDialog)
{
    ui->setupUi(this);

    loadData();
}

MountedVolumesDialog::~MountedVolumesDialog()
{
    MainWindow* castedParent = nullptr;
    if (parent() != nullptr)
    {
        castedParent = dynamic_cast<MainWindow*>(parent());
    }
    if (castedParent != nullptr)
    {
        disconnect(this, &MountedVolumesDialog::leftTreeChange, castedParent, &MainWindow::changeLeftTree);
        disconnect(this, &MountedVolumesDialog::rightTreeChange, castedParent, &MainWindow::changeRightTree);
    }

    delete ui;
}

void MountedVolumesDialog::loadData()
{
    const int ColumnIdxFileSystem = 2;
    const int ColumnIdxFreeSize = 3;
    const int ColumnIdxTotalSize = 4;
    const int ColumnIdxLeft = 5;
    const int ColumnIdxRight = 6;

    ui->tableWidget->clear();

    ui->tableWidget->setColumnCount(7);
    QTableWidgetItem* headerItem = new QTableWidgetItem("Gerät");
    ui->tableWidget->setHorizontalHeaderItem(0, headerItem);
    headerItem = new QTableWidgetItem("Wurzelpfad");
    ui->tableWidget->setHorizontalHeaderItem(1, headerItem);
    headerItem = new QTableWidgetItem("Dateisystem");
    ui->tableWidget->setHorizontalHeaderItem(ColumnIdxFileSystem, headerItem);
    headerItem = new QTableWidgetItem("Freier Speicher");
    ui->tableWidget->setHorizontalHeaderItem(ColumnIdxFreeSize, headerItem);
    headerItem = new QTableWidgetItem("Speicherkapazität");
    ui->tableWidget->setHorizontalHeaderItem(ColumnIdxTotalSize, headerItem);
    headerItem = new QTableWidgetItem("Linker Baum");
    ui->tableWidget->setHorizontalHeaderItem(ColumnIdxLeft, headerItem);
    headerItem = new QTableWidgetItem("Rechter Baum");
    ui->tableWidget->setHorizontalHeaderItem(ColumnIdxRight, headerItem);

    const QIcon jumpIcon = QIcon::hasThemeIcon("go-jump") ? QIcon::fromTheme("go-jump") : QIcon();

    const QList<QStorageInfo> volumes = QStorageInfo::mountedVolumes();
    int row_idx = 0;
    const QLocale loc = locale();

    for (const QStorageInfo& info: volumes)
    {
        if (!info.isValid() || !info.isReady())
        {
            continue;
        }

        ui->tableWidget->setRowCount(row_idx + 1);

        QTableWidgetItem* item = new QTableWidgetItem();
        item->setText(info.displayName());
        item->setFlags(item->flags() & ~ Qt::ItemFlag::ItemIsEditable);
        ui->tableWidget->setItem(row_idx, 0, item);

        item = new QTableWidgetItem();
        item->setText(info.rootPath());
        item->setFlags(item->flags() & ~ Qt::ItemFlag::ItemIsEditable);
        ui->tableWidget->setItem(row_idx, 1, item);

        item = new QTableWidgetItem();
        item->setText(info.fileSystemType());
        item->setFlags(item->flags() & ~ Qt::ItemFlag::ItemIsEditable);
        ui->tableWidget->setItem(row_idx, ColumnIdxFileSystem, item);

        const qint64 free = info.bytesFree();
        const qint64 total = info.bytesTotal();
        const qint64 percent = free * 100 / total;

        item = new QTableWidgetItem();
        item->setText(loc.formattedDataSize(free) + " (" + QString::number(percent) + " %)");
        item->setFlags(item->flags() & ~ Qt::ItemFlag::ItemIsEditable);
        ui->tableWidget->setItem(row_idx, ColumnIdxFreeSize, item);

        item = new QTableWidgetItem();
        item->setText(loc.formattedDataSize(total));
        item->setFlags(item->flags() & ~ Qt::ItemFlag::ItemIsEditable);
        ui->tableWidget->setItem(row_idx, ColumnIdxTotalSize, item);

        QPushButton* btn = new QPushButton("Hierhin wechseln");
        btn->setToolTip("Wechselt das angezeigte Verzeichnis des linken Baumes auf " + info.rootPath());
        if (!jumpIcon.isNull())
        {
            btn->setIcon(jumpIcon);
        }
        connect(btn, &QPushButton::clicked, this, [info, this] {
            emit leftTreeChange(info.rootPath());
        });
        ui->tableWidget->setCellWidget(row_idx, ColumnIdxLeft, btn);

        btn = new QPushButton("Hierhin wechseln");
        btn->setToolTip("Wechselt das angezeigte Verzeichnis des rechten Baumes auf " + info.rootPath());
        if (!jumpIcon.isNull())
        {
            btn->setIcon(jumpIcon);
        }
        connect(btn, &QPushButton::clicked, this, [info, this] {
            emit rightTreeChange(info.rootPath());
        });
        ui->tableWidget->setCellWidget(row_idx, ColumnIdxRight, btn);

        ++row_idx;
    }

    ui->tableWidget->resizeColumnToContents(ColumnIdxFileSystem);
    ui->tableWidget->resizeColumnToContents(ColumnIdxFreeSize);
    ui->tableWidget->resizeColumnToContents(ColumnIdxTotalSize);
    ui->tableWidget->resizeColumnToContents(ColumnIdxLeft);
    ui->tableWidget->resizeColumnToContents(ColumnIdxRight);

    MainWindow* castedParent = nullptr;
    if (parent() != nullptr)
    {
        castedParent = dynamic_cast<MainWindow*>(parent());
    }
    if (castedParent != nullptr)
    {
        connect(this, &MountedVolumesDialog::leftTreeChange, castedParent, &MainWindow::changeLeftTree);
        connect(this, &MountedVolumesDialog::rightTreeChange, castedParent, &MainWindow::changeRightTree);
    }
}

void MountedVolumesDialog::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key::Key_Escape)
    {
        this->close();
    }
}
