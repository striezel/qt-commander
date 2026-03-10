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

    /// Checks whether the given MIME type is a supported movie format which can
    /// be loaded by the movie viewer.
    bool isSupportedMovieFormat(const QMimeType& mimeType) const;

    /// Checks whether the given MIME type is a video format.
    ///
    /// Note that this does not necessarily imply that the format is supported
    /// by the movie viewer. Use isSupportedMovieFormat() to check for support.
    bool isMovieFormat(const QMimeType& mimeType) const;

    /// Checks whether the given MIME type is an audio format.
    bool isAudioFormat(const QMimeType& mimeType) const;

private:
    QMimeDatabase mimeDb;
};

#endif // FILETYPEDETECTION_H
