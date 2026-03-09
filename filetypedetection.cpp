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

void FileTypeDetection::listSupportedImageFormats()
{
    qDebug() << "Supported image formats:";
    for (const auto& element: QImageReader::supportedImageFormats())
    {
        qDebug() << element;
    }

    qDebug() << "Supported mime types:";
    for (const auto& element: QImageReader::supportedMimeTypes())
    {
        qDebug() << element;
    }
}

bool FileTypeDetection::isSupportedImageFormat(const QFileInfo &info) const
{
    return isSupportedImageFormat(getType(info));
}

