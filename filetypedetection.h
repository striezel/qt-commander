#ifndef FILETYPEDETECTION_H
#define FILETYPEDETECTION_H

#include <QFileInfo>
#include <QMimeDatabase>

class FileTypeDetection
{
public:
    FileTypeDetection();

    QMimeType getType(const QFileInfo& info) const;

    void listSupportedImageFormats();

    bool isSupportedImageFormat(const QMimeType& mimeType) const;
    bool isSupportedImageFormat(const QFileInfo& info) const;

private:
    QMimeDatabase mimeDb;
};

#endif // FILETYPEDETECTION_H
