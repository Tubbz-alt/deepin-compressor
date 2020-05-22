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
#ifndef FILEVIWER_H
#define FILEVIWER_H

#include <QFileInfo>
#include <QLineEdit>
#include <DTableView>
#include <DLabel>
#include <QItemDelegate>
#include <QPainter>
#include "myfilesystemmodel.h"
#include <DScrollBar>
#include <QStandardItemModel>
#include <dfiledragserver.h>
#include <dfiledrag.h>
#include "archivemodel.h"
#include <DMenu>
#include "mimetypedisplaymanager.h"
#include "archivesortfiltermodel.h"
#include <DApplicationHelper>
#include <DFontSizeManager>
#include <QUrl>
#include <QReadWriteLock>
#include "openwithdialog/openwithdialog.h"

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

enum PAGE_TYPE {
    PAGE_COMPRESS,
    PAGE_UNCOMPRESS,
};

enum EXTRACT_TYPE {
    EXTRACT_HEAR,
    EXTRACT_TO,
    EXTRACT_DRAG,
    EXTRACT_TEMP,
    EXTRACT_TEMP_OPEN,
    EXTRACT_TEMP_CHOOSE_OPEN,
    EXTRACT_DELETE
};

enum SUBACTION_MODE {
    ACTION_INVALID,
    ACTION_DRAG,
    ACTION_DELETE,
    ACTION_OPEN
};

struct SubActionInfo {
    SubActionInfo()
        : mode(ACTION_INVALID)
    {

    }
    SUBACTION_MODE mode;
    QString archive;
    QString packageFile;
    QStringList ActionFiles;
};

class LogViewHeaderView;

class FirstRowDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    FirstRowDelegate(QObject *parent = nullptr);
    void setPathIndex(int *index);
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void drawFocus(QPainter */*painter*/, const QStyleOptionViewItem &/*option*/,
                           const QRect &/*rect*/) const
    {
    }

    virtual void drawCheck(QPainter */*painter*/, const QStyleOptionViewItem &/*option*/,
                           const QRect &/*rect*/, Qt::CheckState /*state*/) const
    {
    }

protected:

private:
    int *ppathindex;
};

class MyTableView: public DTableView
{
    Q_OBJECT
public:
    MyTableView(QWidget *parent = nullptr);
    void setPreviousButtonVisible(bool visible);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;

    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;

signals:
    void sigdragLeave(QString path);
    void signalDrop(QStringList file);

public slots:
    void slotDragpath(QUrl url);

private:
    // get parent archive::entry pointer
    Archive::Entry *getParentArchiveEntry();
private:
    QPoint dragpos;
    DFileDragServer *s = nullptr;
    QString m_path;

public:
    LogViewHeaderView *header_;
};

struct SortInfo {
    Qt::SortOrder sortOrder;
    int sortRole;
};


class MimesAppsManager;
class fileViewer : public DWidget
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.archive.fileViewer.registry")
public:
    fileViewer(QWidget *parent = nullptr, PAGE_TYPE type = PAGE_COMPRESS);

    void InitUI();
    void InitConnection();

    int getPathIndex();
    void setFileList(const QStringList &files);
    void setSelectFiles(const QStringList &files);
    int getFileCount();
    int getDeFileCount();
    void setDecompressModel(ArchiveSortFilterModel *model);

    QVector<Archive::Entry *> filesAndRootNodesForIndexes(const QModelIndexList &list) const;
    QVector<Archive::Entry *> filesForIndexes(const QModelIndexList &list) const;
    QModelIndexList addChildren(const QModelIndexList &list) const;

    void startDrag(Qt::DropActions supportedActions);

    void deleteCompressFile();
    void resetTempFile();
    void subWindowChangedMsg(const SUBACTION_MODE &mode, const QStringList &msg);

    void upDateArchive(const SubActionInfo &dragInfo);

    MyTableView *getTableView();


public slots:
    void showPlable();
    void onSortIndicatorChanged(int logicalIndex, Qt::SortOrder order);

    void clickedSlot(int index, const QString &text);
    void SubWindowDragMsgReceive(int mode, const QStringList &urls);
protected:
    void resizecolumn();
    void resizeEvent(QResizeEvent *size) override;

protected slots:
    void slotCompressRowDoubleClicked(const QModelIndex index);
    void slotDecompressRowDoubleClicked(const QModelIndex index);
    void slotCompressRePreviousDoubleClicked();
    void slotDecompressRowDelete();

    void showRightMenu(const QPoint &pos);
    void onRightMenuClicked(QAction *action);
    void onRightMenuOpenWithClicked(QAction *action);
    void slotDragLeave(QString path);
    void onDropSlot(QStringList files);
    void slotDeletedFinshedAddStart();


signals:
    void sigFileRemoved(const QStringList &);
    /**
     * @brief sigEntryRemoved
     * @param vectorDel
     * @param isManual,true:by action clicked; false: by message emited.
     */
    void sigEntryRemoved(QVector<Archive::Entry *> &vectorDel, bool isManual);
    void sigFileRemovedFromArchive(const QStringList &, const QString &);
    void sigextractfiles(QVector<Archive::Entry *> fileList, EXTRACT_TYPE type, QString path = "");
    void sigpathindexChanged();
    void sigOpenWith(QVector<Archive::Entry *> fileList, const QString &programma);
    void sigFileAutoCompress(const QStringList &);
    void sigFileAutoCompressToArchive(const QStringList &, const QString &);
private:
    void refreshTableview();

    void restoreHeaderSort(const QString &currentPath);

    void updateAction(const QString &fileType);

    void openWithDialog(const QModelIndex &index);
    void openWithDialog(const QModelIndex &index, const QString &programma);

    void keyPressEvent(QKeyEvent *event) override;
    int popUpDialog(const QString &desc);
    void openTempFile(QString path);

private:
    QLineEdit *pLineEditDir;
    MyTableView *pTableViewFile;

    int m_pathindex;
    MyFileSystemModel *pModel;

    QStandardItemModel *firstmodel;
    QItemSelectionModel *firstSelectionModel = nullptr;

    ArchiveModel *m_decompressmodel;
    ArchiveSortFilterModel *m_sortmodel;
    FirstRowDelegate *pdelegate;
    //MyLabel *plabel;
    QModelIndex m_indexmode;
    QFileInfoList m_curfilelist;

//    QFileInfoList m_parentFileList;
//    QDir m_parentDir;

    bool curFileListModified = true;
    QList<int> m_fileaddindex;
    DMenu *m_pRightMenu = nullptr;
    DMenu *openWithDialogMenu  = nullptr;

    PAGE_TYPE m_pagetype;
    MimeTypeDisplayManager *m_mimetype;

    QMap<QString, SortInfo> sortCache_;
    QAction *deleteAction;

    bool isPromptDelete = false;
    SubActionInfo m_ActionInfo;
    int openFileTempLink = 0;
};

#endif // FILEVIWER_H
