#include "dirutils.h"

#include <QDebug>
#include <QFile>
#include <QString>

bool DirUtils::isParentOf(const QDir& parent, const QDir& potentialChild)
{
    QString parentPath = parent.canonicalPath();
    if (parentPath.isEmpty())
    {
        // Canonical path can be an empty string, if the path contains
        // unresolvable symlinks.
        parentPath = parent.absolutePath();
    }

    QString childPath = potentialChild.canonicalPath();
    if (childPath.isEmpty())
    {
        childPath = potentialChild.absolutePath();
    }

    qDebug() << "parent path:" << parentPath;
    qDebug() << "child path:" << childPath;
    qDebug() << "separator:" << QDir::separator();
#ifdef _WIN32
    // QDir seems to use '/' as separator on Windows, too, so let's check that,
    // too, even if we are on Windows.
    const QChar alternativeSeparator = '/';
#endif
    return (parentPath == childPath)
#ifdef _WIN32
           || childPath.startsWith(parentPath + alternativeSeparator)
#endif
           || childPath.startsWith(parentPath + QDir::separator());
}

bool DirUtils::isSameDir(const QDir &one, const QDir &two)
{
    QString firstPath = one.canonicalPath();
    if (firstPath.isEmpty())
    {
        // Canonical path can be an empty string, if the path contains
        // unresolvable symlinks.
        firstPath = one.absolutePath();
    }

    QString secondPath = two.canonicalPath();
    if (secondPath.isEmpty())
    {
        secondPath = two.absolutePath();
    }

    return firstPath == secondPath;
}

bool DirUtils::copyRecursively(const QDir &source, const QDir &destination)
{
    if (!source.exists())
    {
        qDebug() << "copyRecursively: source does not exist.";
        return false;
    }

    if (destination.exists())
    {
        qDebug() << "copyRecursively: Destination already exists.";
        return false;
    }

    if (!destination.mkdir("."))
    {
        qDebug() << "copyRecursive: Failed to create directory " << destination.absoluteFilePath(".") << ".";
        return false;
    }

    const QFileInfoList list = source.entryInfoList(
        QDir::Filter::AllEntries | QDir::Filter::NoDotAndDotDot | QDir::Filter::Hidden);

    for (const QFileInfo& info: list)
    {
        if (!info.isDir())
        {
            if (!QFile::copy(source.absoluteFilePath(info.fileName()), destination.absoluteFilePath(info.fileName())))
            {
                qDebug() << "copyRecursive: Failed to copy " << source.absoluteFilePath(info.fileName())
                         << " to " << destination.absoluteFilePath(info.fileName());
                return false;
            }
        }
        else
        {
            // Directory has to be handled recursively.
            return copyRecursively(source.absoluteFilePath(info.fileName()),
                                   destination.absoluteFilePath(info.fileName()));
        }
    }

    // Done, all elements were copied.
    return true;
}
