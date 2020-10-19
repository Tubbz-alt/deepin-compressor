/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     gaoxiang <gaoxiang@uniontech.com>
*
* Maintainer: gaoxiang <gaoxiang@uniontech.com>
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
#include "datamodel.h"
#include "uistruct.h"
#include "mimetypedisplaymanager.h"
#include "mimetypes.h"
#include "uitools.h"

#include <QDateTime>
#include <QDebug>
#include <QMimeDatabase>
#include <QIcon>
#include <QCollator>

DataModel::DataModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_pMimetype = new MimeTypeDisplayManager(this);
}

DataModel::~DataModel()
{

}

QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation)

    if (role == Qt::DisplayRole) {
        return g_listColumn[section];           // 返回表头对应列的内容
    } else if (role == Qt::TextAlignmentRole) {
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);  // 表头左对齐
    }

    return QVariant();
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{

    int iRow = index.row();
    int iColumn = index.column();

    FileEntry entry = m_listEntry[iRow];

    // 文件类型     文件夹子文件数目/文件大小
    QMimeType mimetype;
    QString strSize;    // 显示大小（项）

    if (entry.isDirectory) {
        strSize = QString::number(entry.qSize) + " " + tr("item(s)");
    } else {
        strSize = UiTools::humanReadableSize(entry.qSize, 1);
    }

    switch (role) {
    // 显示数据
    case Qt::DisplayRole: {
        switch (iColumn) {
        case DC_Name: {
            return entry.strFileName;
        }
        case DC_Time: {
            return QDateTime::fromTime_t(entry.uLastModifiedTime).toString("yyyy/MM/dd hh:mm:ss");
            // return QLocale().toString(entry.lastModifiedTime, tr("yyyy/MM/dd hh:mm:ss")); // 第二列绑定修改时间格式
        }
        case DC_Type: {
            QMimeType mimetype = entry.isDirectory ? determineMimeType(entry.strFullPath) : determineMimeType(entry.strFileName);
            return m_pMimetype->displayName(mimetype.name());
        }
        case DC_Size: {
            return strSize; // 第四列绑定大小（文件夹为子文件数目，文件为大小）
        }
        }
        break;
    }
    // 数据信息
    case Qt::UserRole: {
        return QVariant::fromValue(entry); // 每一列绑定FileEntry数据
    }
    // 图标数据
    case Qt::DecorationRole: {
        if (iColumn == DC_Name) {
            QMimeDatabase db;
            QIcon icon;
            // 根据类型获取文件类型图标
            entry.isDirectory ? icon = QIcon::fromTheme(db.mimeTypeForName(QStringLiteral("inode/directory")).iconName()).pixmap(24, 24)
                                       : icon = QIcon::fromTheme(db.mimeTypeForFile(entry.strFileName).iconName()).pixmap(24, 24);

            // 如果获取到的图标为空，则默认给一个名称为 "empty" 的图标
            if (icon.isNull()) {
                icon = QIcon::fromTheme("empty").pixmap(24, 24);
            }

            return icon;
        }
        return QVariant();
    }
    }

    return QVariant();

}

int DataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_listEntry.count();
}

int DataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return g_listColumn.count();
}

void DataModel::refreshFileEntry(const QList<FileEntry> &listEntry)
{
    beginResetModel();//在开始添加此函数
    m_listEntry = listEntry;
    endResetModel();  //在结束前添加此函数
}

void DataModel::sort(int column, Qt::SortOrder order)
{
    if (0 > column || 3 < column) {
        return;
    }

    beginResetModel();//在开始添加此函数
    qSort(m_listEntry.begin(), m_listEntry.end(), [&](FileEntry entrya, FileEntry entryb) -> bool {
        //文件与目录分开排序,目录始终在前
        if (entrya.isDirectory && !entryb.isDirectory)
        {
            return true;
        }
        if (!entrya.isDirectory && entryb.isDirectory)
        {
            return false;
        }

        switch (column)
        {
        case DC_Name: {
            if (order == Qt::AscendingOrder) { //升序
                if (QChar::Script_Han == entrya.strFileName.at(0).script()) { //左侧第一个是汉字
                    if (QChar::Script_Han == entryb.strFileName.at(0).script()) { //右侧第一个是汉字
                        QCollator col;
                        return (col.compare(entrya.strFileName, entryb.strFileName) < 0);
                    } else {
                        return false; //右侧第一个不是汉字
                    }
                }
                return QString::compare(entrya.strFileName, entryb.strFileName, Qt::CaseInsensitive) < 0;
            } else { //降序
                if (QChar::Script_Han == entrya.strFileName.at(0).script()) {
                    //左侧第一个是汉字
                    if (QChar::Script_Han == entryb.strFileName.at(0).script()) { //右侧第一个是汉字
                        QCollator col;
                        return (col.compare(entrya.strFileName, entryb.strFileName) > 0);
                    } else {
                        return true; //右侧第一个不是汉字
                    }
                }
                return QString::compare(entrya.strFileName, entryb.strFileName, Qt::CaseInsensitive) > 0;
            }
        }
        case DC_Time: {
            // 比较文件最后一次修改时间
            if (order == Qt::AscendingOrder) {
                return (entrya.uLastModifiedTime < entryb.uLastModifiedTime);
            } else {
                return (entrya.uLastModifiedTime > entryb.uLastModifiedTime);
            }
        }
        case DC_Type: {

            QMimeType mimeLeftType = determineMimeType(entrya.strFullPath);
            QMimeType mimeRightType = determineMimeType(entryb.strFullPath);

            // 比较显示类型
            QCollator col;
            if (order == Qt::AscendingOrder) {
                return (col.compare(m_pMimetype->displayName(mimeLeftType.name()), m_pMimetype->displayName(mimeRightType.name())) < 0);
            } else {
                return (col.compare(m_pMimetype->displayName(mimeLeftType.name()), m_pMimetype->displayName(mimeRightType.name())) > 0);
            }
        }
        case DC_Size: {
            if (order == Qt::AscendingOrder) {
                return (entrya.qSize < entryb.qSize);
            } else {
                return (entrya.qSize > entryb.qSize);
            }
        }
        default:
            return true;
        }
    });
    endResetModel();  //在结束前添加此函数
}