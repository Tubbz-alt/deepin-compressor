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

#ifndef COMPRESSPAGE_H
#define COMPRESSPAGE_H

#include <DWidget>
#include <DPushButton>
#include <QSettings>

DWIDGET_USE_NAMESPACE

class fileViewer;

class CompressPage : public DWidget
{
    Q_OBJECT

public:
    CompressPage(QWidget *parent = nullptr);

    QStringList getCompressFilelist();
    void showDialog();
    int showReplaceDialog(QString name);
    void setRootPathIndex();

signals:
    void sigNextPress();

public slots:
    void onNextPress();
    void onAddfileSlot();
    void onSelectedFilesSlot(const QStringList &files);
    void onRefreshFilelist(const QStringList &filelist);
    void onPathIndexChanged();

    void clearFiles();

signals:
    void sigselectedFiles(const QStringList &files);
    void sigiscanaddfile(bool status);
    void sigFilelistIsEmpty();

    void sigRefreshFileList(const QStringList &files);

private:

    fileViewer *m_fileviewer;
    DPushButton *m_nextbutton;
    QSettings *m_settings;
    QStringList m_filelist;

};

#endif
