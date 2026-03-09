#include "filetypedetection.h"

#include <QImageReader>
#include <QMovie>

FileTypeDetection::FileTypeDetection()
{
}

QMimeType FileTypeDetection::getType(const QFileInfo &info) const
{
    return mimeDb.mimeTypeForFile(info);
}

bool FileTypeDetection::isSupportedImageFormat(const QMimeType& mimeType) const
{
    return QImageReader::supportedMimeTypes().contains(mimeType.name());
}

bool FileTypeDetection::isSupportedImageFormat(const QFileInfo &info) const
{
    return isSupportedImageFormat(getType(info));
}

bool FileTypeDetection::isSupportedMovieFormat(const QMimeType &mimeType) const
{
    return QMovie::supportedFormats().contains(mimeType.name());
}

bool FileTypeDetection::isMovieFormat(const QMimeType &mimeType) const
{
    return mimeType.name().startsWith("video/");
}
