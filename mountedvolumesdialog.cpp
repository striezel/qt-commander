#include "mountedvolumesdialog.h"
#include "ui_mountedvolumesdialog.h"

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
    delete ui;
}

void MountedVolumesDialog::loadData()
{
    const int ColumnIdxFileSystem = 2;
    const int ColumnIdxFreeSize = 3;
    const int ColumnIdxTotalSize = 4;

    ui->tableWidget->clear();

    ui->tableWidget->setColumnCount(5);
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
        ui->tableWidget->setItem(row_idx, 0, item);

        item = new QTableWidgetItem();
        item->setText(info.rootPath());
        ui->tableWidget->setItem(row_idx, 1, item);

        item = new QTableWidgetItem();
        item->setText(info.fileSystemType());
        ui->tableWidget->setItem(row_idx, ColumnIdxFileSystem, item);

        const qint64 free = info.bytesFree();
        const qint64 total = info.bytesTotal();
        const qint64 percent = free * 100 / total;

        item = new QTableWidgetItem();
        item->setText(loc.formattedDataSize(free) + " (" + QString::number(percent) + " %)");
        ui->tableWidget->setItem(row_idx, ColumnIdxFreeSize, item);

        item = new QTableWidgetItem();
        item->setText(loc.formattedDataSize(total));
        ui->tableWidget->setItem(row_idx, ColumnIdxTotalSize, item);

        ++row_idx;
    }

    ui->tableWidget->resizeColumnToContents(ColumnIdxFileSystem);
    ui->tableWidget->resizeColumnToContents(ColumnIdxFreeSize);
    ui->tableWidget->resizeColumnToContents(ColumnIdxTotalSize);
}
