#ifndef CREATEDIRECTORYDIALOG_H
#define CREATEDIRECTORYDIALOG_H

#include <QDialog>

namespace Ui {
class CreateDirectoryDialog;
}

class CreateDirectoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateDirectoryDialog(QWidget *parent = nullptr);
    ~CreateDirectoryDialog();

    /// Gets the directory name that was entered by the user.
    QString directoryName() const;

private:
    Ui::CreateDirectoryDialog *ui;
};

#endif // CREATEDIRECTORYDIALOG_H
