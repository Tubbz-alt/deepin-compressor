/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     dongsen <dongsen@deepin.com>
 *
 * Maintainer: dongsen <dongsen@deepin.com>
 *             AaronZhang <ya.zhang@archermind.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ARCHIVEMODEL_H
#define ARCHIVEMODEL_H


#include "archiveentry.h"

#include <QAbstractItemModel>
#include <QScopedPointer>
#include <QTableView>
#include "mimetypedisplaymanager.h"

class Query;


/**
 * Meta data related to one entry in a compressed archive.
 *
 * This is used for indexing entry properties as numbers
 * and for determining data displaying order in part's view.
 */
enum EntryMetaDataType {
    FullPath,            /**< The entry's file name */
    Timestamp,           /**< The timestamp for the current entry */
    Type,
    Size                /**< The entry's original size */
};

namespace  ArchiveModelDefine {
const int  gTableHeight = 36;
}

class ArchiveModel: public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ArchiveModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    //drag and drop related
    Qt::DropActions supportedDropActions() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
//    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    void reset();
    void createEmptyArchive(const QString &path, const QString &mimeType, QObject *parent);
    KJob *loadArchive(const QString &path, const QString &mimeType, QObject *parent);
    Archive *archive() const;

    QList<int> shownColumns() const;
    QMap<int, QByteArray> propertiesMap() const;

    Archive::Entry *entryForIndex(const QModelIndex &index);
    bool isentryDir(const QModelIndex &index);
    void setPathIndex(int *index);
    void setParentEntry(const QModelIndex &index);
    Archive::Entry *getParentEntry();
    Archive::Entry *getRootEntry();
    /**
     * @brief check if exists archive with fullpath
     * @param fullPath
     * @return
     */
    Archive::Entry *isExists(QString fullPath);
    void setTableView(QTableView *tableview);
    QModelIndex createNoncolumnIndex(const QModelIndex &index) const;

    ExtractJob *extractFile(Archive::Entry *file, const QString &destinationDir, const ExtractionOptions &options = ExtractionOptions()) const;
    ExtractJob *extractFiles(const QVector<Archive::Entry *> &files, const QString &destinationDir, const ExtractionOptions &options = ExtractionOptions()) const;

    PreviewJob *preview(Archive::Entry *file) const;
    OpenJob *open(Archive::Entry *file) const;
    OpenWithJob *openWith(Archive::Entry *file) const;

    AddJob *addFiles(QVector<Archive::Entry *> &entries, const Archive::Entry *destination, const CompressionOptions &options = CompressionOptions());
    AddJob *addFiles(QVector<Archive::Entry *> &entries, const Archive::Entry *destination, ReadOnlyArchiveInterface *pIface = nullptr, const CompressionOptions &options = CompressionOptions());
    MoveJob *moveFiles(QVector<Archive::Entry *> &entries, Archive::Entry *destination, const CompressionOptions &options = CompressionOptions());
    CopyJob *copyFiles(QVector<Archive::Entry *> &entries, Archive::Entry *destination, const CompressionOptions &options = CompressionOptions());
    DeleteJob *deleteFiles(QVector<Archive::Entry *> entries);

    /**
     * @brief insertEntryIcons
     * @param map
     */
    void appendEntryIcons(const QHash<QString, QIcon> &map);
    /**
     * @param password The password to encrypt the archive with.
     * @param encryptHeader Whether to encrypt also the list of files.
     */
    void encryptArchive(const QString &password, bool encryptHeader);

    void countEntriesAndSize();
    qulonglong numberOfFiles() const;
    qulonglong numberOfFolders() const;
    qulonglong uncompressedSize() const;

    bool conflictingEntries(QList<const Archive::Entry *> &conflictingEntries, const QStringList &entries, bool allowMerging) const;

    static bool hasDuplicatedEntries(const QStringList &entries);

    static QMap<QString, Archive::Entry *> entryMap(const QVector<Archive::Entry *> &entries);

    const QHash<QString, QIcon> entryIcons() const;

    QMap<QString, Archive::Entry *> filesToMove;
    QMap<QString, Archive::Entry *> filesToCopy;
    //store the map what key is path,value is entryChild. If open a new view to see the entryChild,you need to store it.
    QMap<QString, Archive::Entry *> mapFilesUpdate;
//    QList<Archive::Entry *> *getLeavesList();
Q_SIGNALS:
    void loadingStarted();
    void loadingFinished(KJob *);
    void extractionFinished(bool success);
    void error(const QString &error, const QString &details);
    void droppedFiles(const QStringList &files, const Archive::Entry *);
    void sigShowLabel() const;


private Q_SLOTS:
    void slotNewEntry(Archive::Entry *entry);
    void slotAddEntry(Archive::Entry *entry);
    void slotListEntry(Archive::Entry *entry);
    void slotLoadingFinished(KJob *job);
    void slotEntryRemoved(const QString &path);
    void slotUserQuery(Query *query);
    void slotCleanupEmptyDirs();

private:
    /**
     * Strips file names that start with './'.
     *
     * For more information, see bug 194241.
     *
     * @param fileName The file name that will be stripped.
     *
     * @return @p fileName without the leading './'
     */
    QString cleanFileName(const QString &fileName);

    void initRootEntry();

    enum InsertBehaviour { NotifyViews, DoNotNotifyViews };
    Archive::Entry *parentFor(const Archive::Entry *entry, InsertBehaviour behaviour = NotifyViews);
    QModelIndex indexForEntry(Archive::Entry *entry);
    static bool compareAscending(const QModelIndex &a, const QModelIndex &b);
    static bool compareDescending(const QModelIndex &a, const QModelIndex &b);
    /**
     * Insert the node @p node into the model, ensuring all views are notified
     * of the change.
     */

    void insertEntry(Archive::Entry *entry, InsertBehaviour behaviour = NotifyViews);
    void newEntry(Archive::Entry *receivedEntry, InsertBehaviour behaviour);

    void traverseAndCountDirNode(Archive::Entry *dir);

    QList<int> m_showColumns;
    QScopedPointer<Archive> m_archive;
    QScopedPointer<Archive::Entry> m_rootEntry;
    QHash<QString, QIcon> m_entryIcons;
    QMap<int, QByteArray> m_propertiesMap;

    QString m_dbusPathName;

    qulonglong m_numberOfFiles;
    qulonglong m_numberOfFolders;
    qulonglong m_uncompressedSize;

    // Whether a file entry has been listed. Used to ensure all relevent columns are shown,
    // since directories might have fewer columns than files.
    bool m_fileEntryListed;

    int *m_ppathindex;
    QTableView *m_tableview;
    MimeTypeDisplayManager *m_mimetype;
    Archive::Entry *m_parent = nullptr;
//    Used to speed up the loading of large archives.
    Archive::Entry *s_previousMatch = nullptr;
    QStringList *s_previousPieces = new QStringList();
};
#endif // ARCHIVEMODEL_H
