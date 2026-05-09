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

#include "checksumdialog.h"
#include "ui_checksumdialog.h"
#include "mainwindow.h"

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

CheckSumDialog::CheckSumDialog(const QString& fileName, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CheckSumDialog)
    , fileName(fileName)
    , createdNewFiles(false)
{
    ui->setupUi(this);

    ui->lineEditFileName->setText(fileName);

    connect(ui->btnCalculate, &QPushButton::clicked, this, &CheckSumDialog::btnCalculateClicked);
    connect(ui->btnExit, &QPushButton::clicked, this, &CheckSumDialog::close);

    connect(ui->rbAlgorithmMd5, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmSha1, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmSha224, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmSha256, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmSha384, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmSha512, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmSha3_224, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmSha3_256, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmSha3_384, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmSha3_512, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmBlake2b160, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmBlake2b256, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmBlake2b384, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmBlake2b512, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmBlake2s128, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmBlake2s160, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmBlake2s224, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);
    connect(ui->rbAlgorithmBlake2s256, &QRadioButton::toggled, this, &CheckSumDialog::rbAlgorithmToggled);

    if (parent != nullptr)
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent);
        connect(this, &CheckSumDialog::hashAlgorithmChanged, castedParent, &MainWindow::selectedHashAlgorithmChanged);
    }
}

CheckSumDialog::~CheckSumDialog()
{
    if (parent() != nullptr)
    {
        MainWindow* castedParent = dynamic_cast<MainWindow*>(parent());
        disconnect(this, &CheckSumDialog::hashAlgorithmChanged, castedParent, &MainWindow::selectedHashAlgorithmChanged);
    }

    delete ui;
}

bool CheckSumDialog::hasCreatedNewFiles() const
{
    return createdNewFiles;
}

void CheckSumDialog::setPreSelectedAlgorithm(const QCryptographicHash::Algorithm algorithm)
{
    switch (algorithm)
    {
    case QCryptographicHash::Algorithm::Md5:
        ui->rbAlgorithmMd5->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Sha1:
        ui->rbAlgorithmSha1->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Sha224:
        ui->rbAlgorithmSha224->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Sha256:
        ui->rbAlgorithmSha256->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Sha384:
        ui->rbAlgorithmSha384->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Sha512:
        ui->rbAlgorithmSha512->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Sha3_224:
        ui->rbAlgorithmSha3_224->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Sha3_256:
        ui->rbAlgorithmSha3_256->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Sha3_384:
        ui->rbAlgorithmSha3_384->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Sha3_512:
        ui->rbAlgorithmSha3_512->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Blake2b_160:
        ui->rbAlgorithmBlake2b160->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Blake2b_256:
        ui->rbAlgorithmBlake2b256->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Blake2b_384:
        ui->rbAlgorithmBlake2b384->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Blake2b_512:
        ui->rbAlgorithmBlake2b512->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Blake2s_128:
        ui->rbAlgorithmBlake2s128->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Blake2s_160:
        ui->rbAlgorithmBlake2s160->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Blake2s_224:
        ui->rbAlgorithmBlake2s224->setChecked(true);
        break;
    case QCryptographicHash::Algorithm::Blake2s_256:
        ui->rbAlgorithmBlake2s256->setChecked(true);
        break;
    default:
        // Algorithm is not implemented in GUI, fall back to SHA-256.
        ui->rbAlgorithmSha256->setChecked(true);
        break;
    }
}

void CheckSumDialog::rbAlgorithmToggled(bool checked)
{
    if (!checked)
    {
        return;
    }
    ui->plainTextEdit->clear();
}

void CheckSumDialog::btnCalculateClicked()
{
    const QCryptographicHash::Algorithm algorithm = getSelectedAlgorithm();
    emit hashAlgorithmChanged(algorithm);
    QCryptographicHash the_hash(algorithm);
    QFile file(fileName);
    if (!file.open(QIODeviceBase::OpenModeFlag::ReadOnly))
    {
        QMessageBox::critical(this, "Fehler beim Öffnen der Datei",
            "Die Datei " + fileName + " konnte nicht zum Lesen geöffnet werden."
            + " Damit kann auch keine Prüfsumme berechnet werden.");
        return;
    }

    const bool success = the_hash.addData(&file);
    file.close();
    if (!success)
    {
        QMessageBox::critical(this, "Fehler beim Öffnen der Datei",
                              "Die Datei " + fileName + " konnte nicht zum Lesen geöffnet werden."
                                  + " Damit kann auch keine Prüfsumme berechnet werden.");

        return;
    }

    const QByteArray real_hash = the_hash.result();
    const QFileInfo info(fileName);
    ui->plainTextEdit->setPlainText(real_hash.toHex() + QStringLiteral(" *") + info.fileName());

    if (!ui->cbGenerateChecksumFile->isChecked())
    {
        return;
    }

    const QString checksumFileName = info.path() + "/" + info.fileName()
                                     + getAlgorithmExtension(algorithm);
    QFile checksumFile(checksumFileName);
    QIODeviceBase::OpenMode flags = QIODeviceBase::OpenModeFlag::WriteOnly
                                    | QIODeviceBase::OpenModeFlag::NewOnly
                                    | QIODeviceBase::OpenModeFlag::Text;
    if (checksumFile.exists())
    {
        if (QMessageBox::question(this, "Prüfsummendatei existiert bereits",
                "Die Datei " + checksumFileName + " existiert bereits. Soll sie überschrieben werden?")
            == QMessageBox::StandardButton::NoButton)
        {
            return;
        }
        flags = QIODeviceBase::OpenModeFlag::WriteOnly
                | QIODeviceBase::OpenModeFlag::Truncate
                | QIODeviceBase::OpenModeFlag::Text;
    }
    if (!checksumFile.open(flags))
    {
        QMessageBox::critical(this, "Fehler beim Erstellen der Prüfsummendatei",
                              "Die Prüfsummendatei " + checksumFileName + " konnte nicht erstellt und zum Schreiben geöffnet werden."
                                  + " Möglicherweise fehlen die Berechtigungen zum Erstellen der Datei.");
        return;
    }
    QTextStream stream(&checksumFile);
    stream << ui->plainTextEdit->toPlainText() << "\n";
    checksumFile.close();
    createdNewFiles = true;
}

QCryptographicHash::Algorithm CheckSumDialog::getSelectedAlgorithm() const
{
    if (ui->rbAlgorithmMd5->isChecked())
        return QCryptographicHash::Algorithm::Md5;
    if (ui->rbAlgorithmSha1->isChecked())
        return QCryptographicHash::Algorithm::Sha1;
    if (ui->rbAlgorithmSha224->isChecked())
        return QCryptographicHash::Algorithm::Sha224;
    if (ui->rbAlgorithmSha256->isChecked())
        return QCryptographicHash::Algorithm::Sha256;
    if (ui->rbAlgorithmSha384->isChecked())
        return QCryptographicHash::Algorithm::Sha384;
    if (ui->rbAlgorithmSha512->isChecked())
        return QCryptographicHash::Algorithm::Sha512;
    if (ui->rbAlgorithmSha3_224->isChecked())
        return QCryptographicHash::Algorithm::Sha3_224;
    if (ui->rbAlgorithmSha3_256->isChecked())
        return QCryptographicHash::Algorithm::Sha3_256;
    if (ui->rbAlgorithmSha3_384->isChecked())
        return QCryptographicHash::Algorithm::Sha3_384;
    if (ui->rbAlgorithmSha3_512->isChecked())
        return QCryptographicHash::Algorithm::Sha3_512;
    if (ui->rbAlgorithmBlake2b160->isChecked())
        return QCryptographicHash::Algorithm::Blake2b_160;
    if (ui->rbAlgorithmBlake2b256->isChecked())
        return QCryptographicHash::Algorithm::Blake2b_256;
    if (ui->rbAlgorithmBlake2b384->isChecked())
        return QCryptographicHash::Algorithm::Blake2b_384;
    if (ui->rbAlgorithmBlake2b512->isChecked())
        return QCryptographicHash::Algorithm::Blake2b_512;
    if (ui->rbAlgorithmBlake2s128->isChecked())
        return QCryptographicHash::Algorithm::Blake2s_128;
    if (ui->rbAlgorithmBlake2s160->isChecked())
        return QCryptographicHash::Algorithm::Blake2s_160;
    if (ui->rbAlgorithmBlake2s224->isChecked())
        return QCryptographicHash::Algorithm::Blake2s_224;
    if (ui->rbAlgorithmBlake2s256->isChecked())
        return QCryptographicHash::Algorithm::Blake2s_256;

    // Usually, we should not get to this place, because one radio button should
    // always be selected. However, as a precaution, let's default to SHA-256.
    return QCryptographicHash::Algorithm::Sha256;
}

QString CheckSumDialog::getAlgorithmExtension(const QCryptographicHash::Algorithm algorithm) const
{
    switch (algorithm)
    {
    case QCryptographicHash::Algorithm::Md5:
        return ".md5";
    case QCryptographicHash::Algorithm::Sha1:
        return ".sha1";
    case QCryptographicHash::Algorithm::Sha224:
        return ".sha224";
    case QCryptographicHash::Algorithm::Sha256:
        return ".sha256";
    case QCryptographicHash::Algorithm::Sha384:
        return ".sha384";
    case QCryptographicHash::Algorithm::Sha512:
        return ".sha512";
    case QCryptographicHash::Algorithm::Sha3_224:
    case QCryptographicHash::Algorithm::Sha3_256:
    case QCryptographicHash::Algorithm::Sha3_384:
    case QCryptographicHash::Algorithm::Sha3_512:
        return ".sha3";
    case QCryptographicHash::Algorithm::Blake2b_160:
    case QCryptographicHash::Algorithm::Blake2b_256:
    case QCryptographicHash::Algorithm::Blake2b_384:
    case QCryptographicHash::Algorithm::Blake2b_512:
    case QCryptographicHash::Algorithm::Blake2s_128:
    case QCryptographicHash::Algorithm::Blake2s_160:
    case QCryptographicHash::Algorithm::Blake2s_224:
    case QCryptographicHash::Algorithm::Blake2s_256:
        return ".b2sum";
    default:
        return ".checksum";
    }
}
