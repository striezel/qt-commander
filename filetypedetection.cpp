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
    // Unlike QImageReader, the image formats returned by QMovie::supportedFormats()
    // are not the MIME types, but just stuff like "gif" or "webp" instead.
    // So we have to do a bit of manual string concatenation to match them with
    // their MIME types. Code assumes that "movie" MIME formats start with
    // either "video/" (like MP4) or "image/" (like GIF).

    const QString name = mimeType.name();
    for (const QByteArray& elem: QMovie::supportedFormats())
    {
        if ((elem == name) || ("image/" + elem == name) || ("video/" + elem == name))
        {
            return true;
        }
    }
    return false;
}

bool FileTypeDetection::isMovieFormat(const QMimeType &mimeType) const
{
    return mimeType.name().startsWith("video/");
}
