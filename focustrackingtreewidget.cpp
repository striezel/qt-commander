#include "focustrackingtreewidget.h"

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
    latestFocusTime = QDateTime::currentDateTimeUtc();

    // Call inherited method to do the actual work.
    this->QTreeWidget::focusInEvent(event);
}
