#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileIconProvider>
#include <QMessageBox>

#include "createdirectorydialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->btnExit, &QPushButton::clicked, this, &MainWindow::close);

    currentDirectoryLeft = QDir::home();
    currentDirectoryRight = QDir::home();

    fillTreeWidget(*(ui->treeWidgetLeft), QDir::homePath());
    fillTreeWidget(*(ui->treeWidgetRight), QDir::homePath());

    connect(ui->treeWidgetLeft, &QTreeWidget::itemDoubleClicked,
            this, &MainWindow::treeItemDoubleClicked);
    connect(ui->treeWidgetRight, &QTreeWidget::itemDoubleClicked,
            this, &MainWindow::treeItemDoubleClicked);

    connect(ui->btnCreateDirectory, &QPushButton::clicked, this, &MainWindow::btnCreateDirectoryClicked);

    // focus on left tree view
    ui->treeWidgetLeft->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fillTreeWidget(QTreeWidget &treeWidget, const QString &path)
{
    if (path.isEmpty())
    {
        return;
    }

    const bool isLeftTree = &treeWidget == ui->treeWidgetLeft;

    QDir dir(path);
    // Canonicalize path to avoid accumulation of "../" sequences.
    if (path.contains(".."))
    {
        const QString canonical = dir.canonicalPath();
        // Canonical path can be empty due to unresolved symlinks. We don't
        // want that.
        if (!dir.isEmpty())
        {
            dir = QDir(dir.canonicalPath());
        }
    }
    dir.setSorting(QDir::SortFlag::Name // sort by name ...
                   | QDir::SortFlag::DirsFirst  // ... and put directories first ...
                   | QDir::SortFlag::IgnoreCase); // ... and ignore casing
    dir.setFilter(QDir::Filter::AllEntries | QDir::Filter::NoDot | QDir::Filter::Hidden);
    const QFileInfoList list = dir.entryInfoList();

    treeWidget.clear();

    QFileIconProvider icon_provider;
    const QIcon directory_icon = icon_provider.icon(QAbstractFileIconProvider::Folder);
    const QIcon file_icon = icon_provider.icon(QAbstractFileIconProvider::File);

    for (const QFileInfo& info: list)
    {
        QStringList data;
        data.append(info.fileName());
        const bool isDirectory = info.isDir();
        data.append(!isDirectory ? QString::number(info.size()) : "Verzeichnis");
        data.append(info.lastModified().toString());

        QTreeWidgetItem* item = new QTreeWidgetItem(data);
        if (info.isDir())
        {
            item->setIcon(0, directory_icon);
        }
        else
        {
            item->setIcon(0, file_icon);
        }
        item->setTextAlignment(1, Qt::AlignRight);
        treeWidget.addTopLevelItem(item);
    }

    // workaround for empty directory -> gets no entries
    if (list.isEmpty())
    {
        QStringList data;
        data.append("..");
        data.append("Verzeichnis");
        data.append("Unbekannt");
        QTreeWidgetItem* item = new QTreeWidgetItem(data);
        item->setIcon(0, directory_icon);
        item->setTextAlignment(1, Qt::AlignRight);
        treeWidget.addTopLevelItem(item);
    }

    if (isLeftTree)
    {
      currentDirectoryLeft = dir;
      ui->lnEdPathLeft->setText(currentDirectoryLeft.absolutePath());
    }
    else
    {
        currentDirectoryRight = dir;
        ui->lnEdPathRight->setText(currentDirectoryRight.absolutePath());
    }
}

bool MainWindow::leftTreeIsLatest() const
{
    return ui->treeWidgetLeft->focusTime() > ui->treeWidgetRight->focusTime();
}

void MainWindow::treeItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (item == nullptr)
        return;

    const bool isLeftTree = item->treeWidget() == ui->treeWidgetLeft;

    const QString new_path = isLeftTree
        ? currentDirectoryLeft.filePath(item->text(0))
        : currentDirectoryRight.filePath(item->text(0));
    const QFileInfo info(new_path);
    if (!info.exists())
    {
        // possibly need to update tree widget here?
        return;
    }
    if (info.isDir())
    {
        fillTreeWidget(*(item->treeWidget()), new_path);
    }
}

void MainWindow::btnCreateDirectoryClicked()
{
    CreateDirectoryDialog* dialog = new CreateDirectoryDialog(this);
    const int choice = dialog->exec();
    if (choice != QDialog::Accepted)
    {
        delete dialog;
        return;
    }

    QString name = dialog->directoryName();
    delete dialog;
    dialog = nullptr;
    if (name.isEmpty() || name.contains("../") || name.contains("..\\"))
    {
        QMessageBox::warning(this, "Ungültiger Verzeichnisname",
                             "Der Verzeichnisname darf nicht leer sein.");
        return;
    }

    QDir& baseDir = leftTreeIsLatest() ? currentDirectoryLeft : currentDirectoryRight;
    const bool success = baseDir.mkdir(name);
    if (!success)
    {
        QMessageBox::critical(
            this, "Fehler beim Erstellen des Verzeichnisses",
            "Das Verzeichnis '" + name + "' konnte nicht erstellt werden.");
        return;
    }

    // refresh corresponding tree widget(s)
    // We might need to refresh both trees, if both tree widgets are showing the
    // same directory.
    if (baseDir.absolutePath() == currentDirectoryLeft.absolutePath())
    {
        fillTreeWidget(*(ui->treeWidgetLeft), currentDirectoryLeft.absolutePath());
    }
    if (baseDir.absolutePath() == currentDirectoryRight.absolutePath())
    {
        fillTreeWidget(*(ui->treeWidgetRight), currentDirectoryRight.absolutePath());
    }

    statusBar()->showMessage("Verzeichnis '" + name + "' wurde erstellt.", 5000);
}
