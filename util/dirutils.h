#ifndef DIRUTILS_H
#define DIRUTILS_H

#include <QDir>

/// Provides some utility functions for use with QDir and/or directory handling
/// in general.
class DirUtils
{
public:
    // Delete constructor to avoid instantiation. This class only has static
    // methods so far, so no instance is needed.
    DirUtils() = delete;

    /// Checks whether the first QDir is a parent directory of the second QDir.
    /// If both QDir instances point to the same directory, then that counts as
    /// parent, too, although that's not a parent in the strict sense.
    static bool isParentOf(const QDir& parent, const QDir& potentialChild);

    /// Checks whether both QDir instances point to the same directory.
    ///
    /// The function is necessary, because the equality operator of QDir also
    /// checks for sort order and filters during comparison, but this is
    /// irrelevant. Only the location in the filesystem is relevant.
    static bool isSameDir(const QDir& one, const QDir& two);

    /// Recursively copies the source directory and its contents (that is, files
    /// and sub-directories) to destination. Destination should not exist yet,
    /// but the parent directory of destination should.
    ///
    /// Returns true, if all files and directories were copied successfully.
    /// Returns false otherwise. In that case a partial copy may have taken
    /// place, so don't assume anything about the existence of the destination
    /// directory.
    static bool copyRecursively(const QDir& source, const QDir& destination);
};

#endif // DIRUTILS_H
