#include "focustrackingtreewidget.h"

#include <QDebug>

FocusTrackingTreeWidget::FocusTrackingTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    latestFocusTime = QDateTime::fromSecsSinceEpoch(0);
}

const QDateTime &FocusTrackingTreeWidget::focusTime() const
{
    return latestFocusTime;
}

void FocusTrackingTreeWidget::focusInEvent(QFocusEvent *event)
{
    qDebug() << "focusInEvent (TreeView): " << this->objectName();
    latestFocusTime = QDateTime::currentDateTimeUtc();
    qDebug() << "UTC: " << latestFocusTime;

    // Call inherited method to do the actual work.
    this->QTreeWidget::focusInEvent(event);
}
