#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileIconProvider>
#include <QMessageBox>

#include "createdirectorydialog.h"
#include "dirutils.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->btnExit, &QPushButton::clicked, this, &MainWindow::close);

    currentDirectoryLeft = QDir::home();
    currentDirectoryRight = QDir::home();

    fillTreeWidget(ui->treeWidgetLeft, QDir::homePath());
    fillTreeWidget(ui->treeWidgetRight, QDir::homePath());

    connect(ui->treeWidgetLeft, &QTreeWidget::itemDoubleClicked,
            this, &MainWindow::treeItemDoubleClicked);
    connect(ui->treeWidgetRight, &QTreeWidget::itemDoubleClicked,
            this, &MainWindow::treeItemDoubleClicked);

    connect(ui->btnCreateDirectory, &QPushButton::clicked, this, &MainWindow::btnCreateDirectoryClicked);
    connect(ui->btnRemove, &QPushButton::clicked, this, &MainWindow::btnRemoveClicked);
    connect(ui->btnMove, &QPushButton::clicked, this, &MainWindow::btnMoveClicked);
    connect(ui->btnCopy, &QPushButton::clicked, this, &MainWindow::btnCopyClicked);
    connect(ui->btnView, &QPushButton::clicked, this, &MainWindow::btnViewClicked);

    connect(ui->actionRefresh, &QAction::triggered, this, &MainWindow::actionRefreshTriggered);

    // focus on left tree view
    ui->treeWidgetLeft->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fillTreeWidget(QTreeWidget* treeWidget, const QString &path)
{
    if ((treeWidget == nullptr) || path.isEmpty())
    {
        return;
    }

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

    treeWidget->clear();

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
        treeWidget->addTopLevelItem(item);
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
        treeWidget->addTopLevelItem(item);
    }

    const bool isLeftTree = treeWidget == ui->treeWidgetLeft;
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

    // Adjust width of column for file size.
    treeWidget->resizeColumnToContents(1);
    // Adjust width of column for modification date.
    treeWidget->resizeColumnToContents(2);
    // Column for file name is not adjusted, because that can get way out of
    // hand with longer file names, triggering horizontal scroll bars.
}

bool MainWindow::leftTreeIsLatest() const
{
    return ui->treeWidgetLeft->focusTime() > ui->treeWidgetRight->focusTime();
}

QTreeWidget *MainWindow::latestTreeWidget() const
{
    return leftTreeIsLatest() ? ui->treeWidgetLeft : ui->treeWidgetRight;
}

QTreeWidget *MainWindow::otherTreeWidget() const
{
    return leftTreeIsLatest() ? ui->treeWidgetRight : ui->treeWidgetLeft;
}

const QDir &MainWindow::currentDirectory() const
{
    return leftTreeIsLatest() ? currentDirectoryLeft : currentDirectoryRight;
}

const QDir& MainWindow::otherDirectory() const
{
    return leftTreeIsLatest() ? currentDirectoryRight : currentDirectoryLeft;
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
        fillTreeWidget(item->treeWidget(), new_path);
    }
}

void MainWindow::btnRemoveClicked()
{
    QTreeWidget* treeWidget = latestTreeWidget();
    const QList<QTreeWidgetItem*> selection = treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        return;
    }
    QTreeWidgetItem* item = selection.at(0);

    const QString name = item->text(0);

    // Do not operate on parent directory!
    if (name == "..")
    {
        QMessageBox::critical(
            this, "Fehler",
            "Löschoperation kann nicht auf \"..\" ausgeführt werden!");
        return;
    }

    // Ask user whether the file / directory shall really be deleted.
    const int answer = QMessageBox::question(
        this, "Wirklich löschen?", "Soll \"" + name + "\" wirklich gelöscht werden?");
    if (answer != QMessageBox::Yes)
    {
        return;
    }
    QDir& baseDir = treeWidget == ui->treeWidgetLeft
                        ? currentDirectoryLeft
                        : currentDirectoryRight;
    const QFileInfo info(baseDir.absoluteFilePath(name));
    bool success = false;
    if (info.isFile())
    {
        success = baseDir.remove(name);
    }
    else
    {
        success = baseDir.rmdir(name);
    }
    if (!success)
    {
        QString message = "\"" + name + "\" konnte nicht gelöscht werden.";
        if (info.isFile())
        {
            message = "Die Datei " + message;
        }
        else
        {
            message = "Das Verzeichnis " + message
                      + "\n\nMöglicherweise ist das Verzeichnis nicht leer.";
        }
        QMessageBox::critical(this, "Fehler beim Löschen", message);
        return;
    }

    // Delete item from tree view.
    int index = treeWidget->indexOfTopLevelItem(item);
    QTreeWidgetItem* toDelete = treeWidget->takeTopLevelItem(index);
    delete toDelete;
    toDelete = nullptr;

    // If both tree widgets show the same directory, then the other widget needs
    // to be updated, too. That is, we have to remove the corresponding item
    // from the other tree widget, too.
    if (baseDir.absolutePath() == otherDirectory().absolutePath())
    {
        QTreeWidget* other = otherTreeWidget();
        const QList<QTreeWidgetItem*> found = other->findItems(
            name, Qt::MatchCaseSensitive | Qt::MatchFixedString);
        if (found.count() != 1)
        {
            qDebug() << "Found " << found.count() << " items, but one was expected.";
            return;
        }
        const int indexToTake = other->indexOfTopLevelItem(found.at(0));
        QTreeWidgetItem* toDeleteOther = other->takeTopLevelItem(indexToTake);
        delete toDeleteOther;
        toDeleteOther = nullptr;
    }
    // ... or when the other tree shows a subdirectory of the deleted directory,
    // then the other tree has to change "upwards" to the directory which still
    // exists.
    else if (DirUtils::isParentOf(baseDir.absoluteFilePath(name), otherDirectory()))
    {
        fillTreeWidget(otherTreeWidget(), baseDir.absolutePath());
    }

    statusBar()->showMessage("'" + name + "' wurde gelöscht.", 5000);
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
        fillTreeWidget(ui->treeWidgetLeft, currentDirectoryLeft.absolutePath());
    }
    if (baseDir.absolutePath() == currentDirectoryRight.absolutePath())
    {
        fillTreeWidget(ui->treeWidgetRight, currentDirectoryRight.absolutePath());
    }

    statusBar()->showMessage("Verzeichnis '" + name + "' wurde erstellt.", 5000);
}

void MainWindow::btnMoveClicked()
{
    QTreeWidget* treeWidget = latestTreeWidget();
    const QList<QTreeWidgetItem*> selection = treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        return;
    }
    QTreeWidgetItem* item = selection.at(0);

    const QString name = item->text(0);

    // Do not operate on parent directory!
    if (name == "..")
    {
        QMessageBox::critical(
            this, "Fehler",
            "Verschiebeoperation kann nicht auf \"..\" ausgeführt werden!");
        return;
    }

    if (DirUtils::isSameDir(currentDirectoryLeft, currentDirectoryRight))
    {
        QMessageBox::warning(
            this, "Verschieben in gleiches Verzeichnis nicht möglich",
            "Die beiden Bäume zeigen auf das gleiche Verzeichnis (\""
                + currentDirectoryLeft.absolutePath() + "\"). Ein Verschieben ist daher nicht möglich.");
        return;
    }

    const QString source = currentDirectory().absoluteFilePath(name);
    const QString destination = otherDirectory().absoluteFilePath(name);
    qDebug() << "Source:      " << source;
    qDebug() << "Destination: " << destination;
    // Move with QFile::rename(). Despite the name QFile it also works for
    // directories, so it's universal.
    if (!QFile::rename(source, destination))
    {
        QMessageBox::critical(
            this, "Fehler beim Verschieben",
            "Das Element '" + name + "' konnte nicht verschoben werden.");
        return;
    }

    // Delete item from current tree view.
    const int index = treeWidget->indexOfTopLevelItem(item);
    QTreeWidgetItem* toDelete = treeWidget->takeTopLevelItem(index);
    delete toDelete;
    toDelete = nullptr;

    // Refresh other tree view.
    fillTreeWidget(otherTreeWidget(), otherDirectory().absolutePath());

    statusBar()->showMessage(
        "'" + name + "' wurde nach " + otherDirectory().absolutePath()
            + " verschoben.", 5000);
}

void MainWindow::btnCopyClicked()
{
    QTreeWidget* treeWidget = latestTreeWidget();
    const QList<QTreeWidgetItem*> selection = treeWidget->selectedItems();
    if (selection.isEmpty())
    {
        return;
    }
    QTreeWidgetItem* item = selection.at(0);

    const QString name = item->text(0);

    // Do not operate on parent directory!
    if (name == "..")
    {
        QMessageBox::critical(
            this, "Fehler",
            "Kopieroperation kann nicht auf \"..\" ausgeführt werden!");
        return;
    }

    if (DirUtils::isSameDir(currentDirectoryLeft, currentDirectoryRight))
    {
        QMessageBox::warning(
            this, "Kopieren in gleiches Verzeichnis nicht möglich",
            "Die beiden Bäume zeigen auf das gleiche Verzeichnis (\""
                + currentDirectoryLeft.absolutePath() + "\"). Ein Kopieren ist daher nicht möglich.");
        return;
    }

    const QString source = currentDirectory().absoluteFilePath(name);
    const QString destination = otherDirectory().absoluteFilePath(name);
    qDebug() << "Source:      " << source;
    qDebug() << "Destination: " << destination;
    const QFileInfo info(source);
    if (info.isFile())
    {
        // Copy files with QFile::copy().
        if (!QFile::copy(source, destination))
        {
            QMessageBox::critical(
                this, "Fehler beim Kopieren",
                "Die Datei '" + name + "' konnte nicht kopiert werden.");
            return;
        }
    }
    else
    {
        // Directories have to be copied manually and recursively.
        if (!DirUtils::copyRecursively(source, destination))
        {
            QMessageBox::critical(
                this, "Fehler beim Kopieren",
                "Das Verzeichnis '" + name + "' konnte nicht kopiert werden.");
            // Still need to update the other widget, if at least one directory
            // was created.
            const QDir partialCopy(destination);
            if (partialCopy.exists())
            {
                fillTreeWidget(otherTreeWidget(), otherDirectory().absolutePath());
            }
            return;
        }
    }

    // Refresh other tree view.
    fillTreeWidget(otherTreeWidget(), otherDirectory().absolutePath());

    statusBar()->showMessage(
        "'" + name + "' wurde nach " + otherDirectory().absolutePath()
            + " kopiert.", 5000);
}

void MainWindow::btnViewClicked()
{
    QMessageBox::information(
        this, "Noch nicht implementiert", "Diese Funktionalität ist noch nicht"
            + QString(" implementiert. Versuche es später nochmal mit einer neueren Programmversion."));
}

void MainWindow::actionRefreshTriggered()
{
    const QString path = currentDirectory().absolutePath();
    fillTreeWidget(latestTreeWidget(), path);

    statusBar()->showMessage("Ansicht für Verzeichnis '" + path + "' ("
                                 + (leftTreeIsLatest() ? "links" : "rechts")
                                 + ") wurde aktualisiert.", 5000);
}
