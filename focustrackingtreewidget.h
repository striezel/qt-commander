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
    // focusInEvent() is protacted in base class, so let's keep it that way.
    void focusInEvent(QFocusEvent* event) override;

private:
    QDateTime latestFocusTime;
};

#endif // FOCUSTRACKINGTREEWIDGET_H
