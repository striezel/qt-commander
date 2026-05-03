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

#include "fileinfowindow.h"
#include "ui_fileinfowindow.h"

#include <QFileInfo>
#if defined(__has_include)
  #if __has_include(<QtVersionChecks>)
    #include <QtVersionChecks>
  #elif __has_include(<QtGlobal>)
    #include <QtGlobal>
  #endif
#endif
#if defined(_WIN32) && (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
#include <QNtfsPermissionCheckGuard>
#endif
#include <QStorageInfo>

FileInfoWindow::FileInfoWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FileInfoWindow)
{
    ui->setupUi(this);

    connect(ui->actionClose, &QAction::triggered, this, &FileInfoWindow::close);
}

FileInfoWindow::~FileInfoWindow()
{
    delete ui;
}

void FileInfoWindow::loadInformation(const QString &filePath)
{
#if defined(_WIN32) && (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
    // NtfsPermissionCheckGuard is only available in Qt 6.6 or later. For older
    // versions there is another (now deprecated) mechanism to enable full
    // NFTS permission checks, but since I'm using a newer Qt version on Windows
    // I will not implement it here.
    QNtfsPermissionCheckGuard guard;
#endif

    const QFileInfo info(filePath);

    ui->lblFileName->setText(info.fileName());

    const QLocale loc = locale();
    const qint64 file_size = info.size();
    ui->lblSizeValue->setText(loc.formattedDataSize(file_size)
                              + " (" + loc.toString(file_size)
                              + (file_size != 1 ? " Bytes)" : " Byte)"));
    const QStorageInfo storage(filePath);
    if (!storage.isValid() || !storage.isReady())
    {
        ui->lblAvailableValue->setText("unbekannt");
    }
    else
    {
        const qint64 free = storage.bytesFree();
        const qint64 total = storage.bytesTotal();
        // The value of total can be zero on virtual file systems (like /proc on
        // Linux systems), so a division by zero has to be avoided here.
        const qint64 percent = total != 0 ? free * 100 / total : 0;
        ui->lblAvailableValue->setText(
            loc.formattedDataSize(free) + " / "
            + loc.formattedDataSize(total)
            + " (" + QString::number(percent) + " %)"
            );
    }

    // On systems where owner id and group id are not supported or files do not
    // have owners or groups, the id will be -2, cast to uint, according to Qt
    // documentation. So let's check that here.
    constexpr uint NoId = static_cast<uint>(-2);
    QString owner = info.owner();
    if (owner.isEmpty())
    {
        owner = "<unbekannt>";
    }
    const uint owner_id = info.ownerId();
    if (owner_id != NoId)
    {
        ui->lblOwnerValue->setText(owner + " (" + QString::number(owner_id) + ")");
    }
    else
    {
        ui->lblOwnerValue->setText(info.owner());
    }
    const uint group_id = info.groupId();
    if (group_id != NoId)
    {
        ui->lblGroupValue->setText(info.group() + " (" + QString::number(group_id) + ")");
    }
    else
    {
        ui->lblGroupValue->setText(info.group());
    }

    const QFileDevice::Permissions permissions = info.permissions();

    ui->cbUserRead->setChecked(permissions.testFlag(QFileDevice::Permission::ReadUser));
    ui->cbUserWrite->setChecked(permissions.testFlag(QFileDevice::Permission::WriteUser));
    ui->cbUserExec->setChecked(permissions.testFlag(QFileDevice::Permission::ExeUser));

    ui->cbGroupRead->setChecked(permissions.testFlag(QFileDevice::Permission::ReadGroup));
    ui->cbGroupWrite->setChecked(permissions.testFlag(QFileDevice::Permission::WriteGroup));
    ui->cbGroupExec->setChecked(permissions.testFlag(QFileDevice::Permission::ExeGroup));

    ui->cbOtherRead->setChecked(permissions.testFlag(QFileDevice::Permission::ReadOther));
    ui->cbOtherWrite->setChecked(permissions.testFlag(QFileDevice::Permission::WriteOther));
    ui->cbOtherExec->setChecked(permissions.testFlag(QFileDevice::Permission::ExeOther));

    // Some file systems do not provide a birth time, so we have to distinguish
    // between cases where it is available (i. e. valid) and when it is not
    // available (i. e. the returned QDateTime is invalid).
    const QDateTime birthTime = info.birthTime();
    if (birthTime.isValid())
    {
        ui->lblBirthValue->setText(loc.toString(birthTime, QLocale::LongFormat));
    }
    else
    {
        ui->lblBirthValue->setText("Datum nicht verfügbar");
    }
    ui->lblModifiedValue->setText(loc.toString(info.lastModified(), QLocale::LongFormat));
    ui->lblAccessedValue->setText(loc.toString(info.lastRead(), QLocale::LongFormat));
}

void FileInfoWindow::closeEvent(QCloseEvent *event)
{
    // ensure deletion
    this->deleteLater();

    // do the usual event handling inherited from base class
    this->QMainWindow::closeEvent(event);
}
