#ifndef MOUNTEDVOLUMESDIALOG_H
#define MOUNTEDVOLUMESDIALOG_H

#include <QDialog>

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

private:
    Ui::MountedVolumesDialog *ui;
};

#endif // MOUNTEDVOLUMESDIALOG_H
