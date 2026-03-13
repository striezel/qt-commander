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
