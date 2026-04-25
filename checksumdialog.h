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

#ifndef CHECKSUMDIALOG_H
#define CHECKSUMDIALOG_H

#include <QCryptographicHash>
#include <QDialog>
#include <QString>

namespace Ui {
class CheckSumDialog;
}

class CheckSumDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckSumDialog(const QString& fileName, QWidget *parent = nullptr);
    ~CheckSumDialog();

    /// Whether the user created new files during the dialog execution.
    bool hasCreatedNewFiles() const;
private slots:
    void rbAlgorithmToggled(bool checked);
    void btnCalculateClicked();
private:
    Ui::CheckSumDialog *ui;

    QString fileName;
    bool createdNewFiles;

    QCryptographicHash::Algorithm getSelectedAlgorithm() const;
    QString getAlgorithmExtension(const QCryptographicHash::Algorithm algorithm) const;
};

#endif // CHECKSUMDIALOG_H
