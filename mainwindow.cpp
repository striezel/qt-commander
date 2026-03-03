#include "mainwindow.h"
#include "./ui_mainwindow.h"

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

#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    // QIcon::fromTheme() with enum paramater is only available from Qt 6.7.0
    // onwards. For older Qt versions we have to use the QString overload.
    const QIcon directory_icon = QIcon::fromTheme(QIcon::ThemeIcon::FolderOpen);
    const QIcon file_icon = QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew);
#else
    const QIcon directory_icon = QIcon::fromTheme("folder-open");
    const QIcon file_icon = QIcon::fromTheme("document-new");
#endif

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
