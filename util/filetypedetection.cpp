/*
 -------------------------------------------------------------------------------
    This file is part of the Qt Commander file manager.
    Copyright (C) 2026  Dirk Stolle

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------------
*/

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

bool FileTypeDetection::isAudioFormat(const QMimeType &mimeType) const
{
    return mimeType.name().startsWith("audio/");
}
