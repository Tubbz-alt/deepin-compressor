#ifndef SINGLEFILEPLUGIN_H
#define SINGLEFILEPLUGIN_H

#include "archiveinterface.h"
#include "libsinglefileplugin_global.h"

class  LibSingleFileInterface : public ReadOnlyArchiveInterface
{
    Q_OBJECT

public:
    LibSingleFileInterface(QObject *parent, const QVariantList &args);
    ~LibSingleFileInterface() ;

    bool list(bool isbatch = false) ;
    bool testArchive() ;
    bool extractFiles(const QVector<Archive::Entry *> &files, const QString &destinationDirectory, const ExtractionOptions &options) ;
    void cleanIfCanceled()override;
protected:
    const QString uncompressedFileName() const;
    QString overwriteFileName(QString &filename);

    QString m_mimeType;
    QStringList m_possibleExtensions;
};

#endif // SINGLEFILEPLUGIN_H
