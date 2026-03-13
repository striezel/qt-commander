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

#ifndef FOCUSTRACKINGTREEWIDGET_H
#define FOCUSTRACKINGTREEWIDGET_H

#include <QDateTime>
#include <QFocusEvent>
#include <QTreeWidget>

/// This class is a QTreeWidget which tracks when it got focussed.
class FocusTrackingTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit FocusTrackingTreeWidget(QWidget *parent = nullptr);

    const QDateTime& focusTime() const;
protected:
    // focusInEvent() is protected in base class, so let's keep it that way.
    void focusInEvent(QFocusEvent* event) override;

private:
    QDateTime latestFocusTime;
};

#endif // FOCUSTRACKINGTREEWIDGET_H
