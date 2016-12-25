
#include "misc.hpp"

#include <QDataStream>
#include <QDir>
#include <QFileInfo>
#include <QTemporaryFile>

#include "project.hpp"


QString copyFileToPrivateMediaLocation(const ProjectInfo& prjInfo, const QString& path)
{
    const QFileInfo originalFileInfo(path);
    const QString extension = originalFileInfo.completeSuffix();
    const QString mediaLocation = prjInfo.getInternalLocation(ProjectInfo::PrivateMultimedia);
    const QString newFilePath = QString("%1/XXXXXX.%2").arg(mediaLocation).arg(extension);

    QDir().mkpath(mediaLocation);

    QFile originalFile(path);
    originalFile.open(QFile::ReadOnly);

    QTemporaryFile newFile(newFilePath);
    newFile.setAutoRemove(false);
    newFile.open();

    bool work = true;
    do
    {
        const int bufferSize = 1024*1024;
        char data[bufferSize];
        const quint64 read = originalFile.read(data, bufferSize);

        if (read > 0)
            newFile.write(data, read);

        work = read == bufferSize;
    }
    while(work);

    return newFilePath;
}
