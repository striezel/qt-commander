#include "createdirectorydialog.h"
#include "ui_createdirectorydialog.h"

CreateDirectoryDialog::CreateDirectoryDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateDirectoryDialog)
{
    ui->setupUi(this);

    ui->lnEdName->setFocus();
}

CreateDirectoryDialog::~CreateDirectoryDialog()
{
    delete ui;
}

QString CreateDirectoryDialog::directoryName() const
{
    return ui->lnEdName->text().trimmed();
}
