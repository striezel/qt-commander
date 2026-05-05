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

#ifndef FILETYPEDETECTION_H
#define FILETYPEDETECTION_H

#include <QFileInfo>
#include <QMimeDatabase>

class FileTypeDetection
{
public:
    FileTypeDetection() = default;

    /// Gets the MIME type of a file.
    QMimeType getType(const QFileInfo& info) const;

    /// Checks whether the given MIME type is a supported image format which can
    /// be loaded by the image viewer.
    bool isSupportedImageFormat(const QMimeType& mimeType) const;

    /// Checks whether the given file contains a supported image format which
    /// can be loaded by the image viewer.
    bool isSupportedImageFormat(const QFileInfo& info) const;

    /// Checks whether the given MIME type is a supported movie format which can
    /// be loaded by the movie viewer.
    bool isSupportedMovieFormat(const QMimeType& mimeType) const;

    /// Checks whether the given MIME type is a supported video format which can
    /// be loaded by the video player. Detection may not be 100 % correct,
    /// because videos can be in a container format and the actual codecs may
    /// be different.
    bool isSupportedVideoFormat(const QMimeType& mimeType) const;

    /// Checks whether the given MIME type is an audio format.
    static bool isAudioFormat(const QMimeType& mimeType);

    /// Checks whether the given MIME type is a video format.
    static bool isVideoFormat(const QMimeType& mimeType);

    /// Checks whether the given MIME type is PDF.
    static bool isPdf(const QMimeType& mimeType);
private:
    QMimeDatabase mimeDb;
};

#endif // FILETYPEDETECTION_H
