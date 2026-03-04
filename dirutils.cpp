#include "dirutils.h"

#include <QDebug>
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
