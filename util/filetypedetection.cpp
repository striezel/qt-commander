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
#include <QMediaFormat>
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

bool FileTypeDetection::isSupportedVideoFormat(const QMimeType &mimeType) const
{
    if (!mimeType.name().startsWith("video/"))
    {
        return false;
    }

    const QMap<QString, QMediaFormat::FileFormat> formatMapper {
        { "video/x-ms-wmv", QMediaFormat::FileFormat::WMV },
        { "video/x-msvideo", QMediaFormat::FileFormat::AVI },
        { "video/x-matroska", QMediaFormat::FileFormat::Matroska },
        { "video/mp4", QMediaFormat::FileFormat::MPEG4 },
        { "video/ogg", QMediaFormat::FileFormat::Ogg },
        { "video/quicktime", QMediaFormat::FileFormat::QuickTime },
        { "video/webm", QMediaFormat::FileFormat::WebM }
    };

    if (!formatMapper.contains(mimeType.name()))
    {
        return false;
    }
    const QMediaFormat::FileFormat fileFormat = formatMapper.contains(mimeType.name())
                                                    ? formatMapper[mimeType.name()]
                                                    : QMediaFormat::FileFormat::UnspecifiedFormat;

    QMediaFormat format;
    const QList<QMediaFormat::FileFormat> supportedFileFormats = format.supportedFileFormats(QMediaFormat::ConversionMode::Decode);
    if (supportedFileFormats.contains(fileFormat))
    {
        return true;
    }

    const QMap<QString, QMediaFormat::VideoCodec> codecMapper {
        { "video/AV1", QMediaFormat::VideoCodec::AV1 },
        { "video/H264", QMediaFormat::VideoCodec::H264 },
        { "video/H265", QMediaFormat::VideoCodec::H265 },
        { "video/mp4", QMediaFormat::VideoCodec::MPEG4 },
        { "video/ogg", QMediaFormat::VideoCodec::Theora },
        { "video/VP8", QMediaFormat::VideoCodec::VP8 },
        { "video/VP9", QMediaFormat::VideoCodec::VP9 },
        { "video/x-ms-wmv", QMediaFormat::VideoCodec::WMV }
    };

    const QMediaFormat::VideoCodec codec = codecMapper.contains(mimeType.name())
                                               ? codecMapper[mimeType.name()]
                                               : QMediaFormat::VideoCodec::Unspecified;

    const QList<QMediaFormat::VideoCodec> supportedVideoCodecs = format.supportedVideoCodecs(QMediaFormat::ConversionMode::Decode);
    return supportedVideoCodecs.contains(codec);
}

bool FileTypeDetection::isMovieFormat(const QMimeType &mimeType) const
{
    return mimeType.name().startsWith("video/");
}

bool FileTypeDetection::isAudioFormat(const QMimeType &mimeType) const
{
    return mimeType.name().startsWith("audio/");
}

bool FileTypeDetection::isVideoFormat(const QMimeType &mimeType) const
{
    return mimeType.name().startsWith("video/");
}
