#ifndef FILETYPEDETECTION_H
#define FILETYPEDETECTION_H

#include <QFileInfo>
#include <QMimeDatabase>

class FileTypeDetection
{
public:
    FileTypeDetection();

    /// Gets the MIME type of a file.
    QMimeType getType(const QFileInfo& info) const;

    /// Checks whether the given MIME type is a supported image format which can
    /// be loaded by the image viewer.
    bool isSupportedImageFormat(const QMimeType& mimeType) const;

    /// Checks whether the given file contains a supported image format which
    /// can be loaded by the image viewer.
    bool isSupportedImageFormat(const QFileInfo& info) const;

private:
    QMimeDatabase mimeDb;
};

#endif // FILETYPEDETECTION_H
