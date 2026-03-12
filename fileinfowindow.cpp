#include "fileinfowindow.h"
#include "ui_fileinfowindow.h"

#include <QFileInfo>
#include <QtVersionChecks>
#if defined(_WIN32) && (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
#include <QNtfsPermissionCheckGuard>
#endif

FileInfoWindow::FileInfoWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FileInfoWindow)
{
    ui->setupUi(this);
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

    qDebug() << "permissions as int:" << permissions.toInt();
    qDebug() << "0777 as int:" << 0777;
    qDebug() << "permissions & 0777:" << (permissions.toInt() & 0777);
    ui->lblOctalValue->setText("0" + QString::number(permissions.toInt() & 0777, 8));
#if defined(_WIN32)
    // Octal permission display does not seem to work properly on Windows, and
    // I don't know why - yet. Therefore it's not shown on Windows.
    // TODO: Fix this one.
    ui->lblOctal->hide();
    ui->lblOctalValue->hide();
#endif
}

void FileInfoWindow::closeEvent(QCloseEvent *event)
{
    // ensure deletion
    this->deleteLater();

    // do the usual event handling inherited from base class
    this->QMainWindow::closeEvent(event);
}
