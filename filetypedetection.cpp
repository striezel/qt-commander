#include "filetypedetection.h"

#include <QImageReader>

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

