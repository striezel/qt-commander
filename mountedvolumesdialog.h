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

#ifndef MOUNTEDVOLUMESDIALOG_H
#define MOUNTEDVOLUMESDIALOG_H

#include <QDialog>
#include <QKeyEvent>

namespace Ui {
class MountedVolumesDialog;
}

class MountedVolumesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MountedVolumesDialog(QWidget *parent = nullptr);
    ~MountedVolumesDialog();

    void loadData();
protected:
    void keyPressEvent(QKeyEvent* event) override;
private:
    Ui::MountedVolumesDialog *ui;
};

#endif // MOUNTEDVOLUMESDIALOG_H
