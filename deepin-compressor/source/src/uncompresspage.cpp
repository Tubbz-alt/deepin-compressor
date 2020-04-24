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

#include <QVBoxLayout>
#include <QDebug>
#include <QFile>
#include <QUrl>
#include <DStandardPaths>
#include <DMessageManager>
#include <DDialog>

#include "uncompresspage.h"
#include "utils.h"


DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

UnCompressPage::UnCompressPage(QWidget *parent)
    : DWidget(parent)
{
    m_pathstr = "~/Desktop";
    m_fileviewer = new fileViewer(this, PAGE_UNCOMPRESS);
    m_nextbutton = new DPushButton(tr("Extract"), this);
    m_nextbutton->setMinimumSize(340, 36);

    QHBoxLayout *contentLayout = new QHBoxLayout;
    contentLayout->addWidget(m_fileviewer);

    m_extractpath = new DCommandLinkButton(tr("Extract to:") + " ~/Desktop");
//    m_extractpath->setFont(DFontSizeManager::instance()->get(DFontSizeManager::T8));
    DFontSizeManager::instance()->bind(m_extractpath, DFontSizeManager::T8);

    QHBoxLayout *buttonlayout = new QHBoxLayout;
    buttonlayout->addStretch(1);
    buttonlayout->addWidget(m_nextbutton, 2);
    buttonlayout->addStretch(1);

    QHBoxLayout *pathlayout = new QHBoxLayout;
    pathlayout->addStretch(1);
    pathlayout->addWidget(m_extractpath, 2, Qt::AlignCenter);
    pathlayout->addStretch(1);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addLayout(contentLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(pathlayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(buttonlayout);
    mainLayout->setStretchFactor(contentLayout, 9);
    mainLayout->setStretchFactor(pathlayout, 1);
    mainLayout->setStretchFactor(buttonlayout, 1);
    mainLayout->setContentsMargins(12, 1, 20, 20);

    setBackgroundRole(DPalette::Base);

    connect(m_nextbutton, &DPushButton::clicked, this, &UnCompressPage::oneCompressPress);
    connect(m_extractpath, &DPushButton::clicked, this, &UnCompressPage::onPathButoonClicked);
    connect(m_fileviewer, &fileViewer::sigextractfiles, this, &UnCompressPage::onextractfilesSlot);
    connect(m_fileviewer, &fileViewer::sigOpenWith,     this, &UnCompressPage::onextractfilesOpenSlot);
    connect(m_fileviewer, &fileViewer::sigFileRemoved, this, &UnCompressPage::onRefreshFilelist);

    connect(m_fileviewer, &fileViewer::sigFileAutoCompress, this, &UnCompressPage::onAutoCompress);
    connect(this, &UnCompressPage::subWindowTipsPopSig, m_fileviewer, &fileViewer::SubWindowDragMsgReceive);

    connect(m_fileviewer, &fileViewer::sigFileRemovedFromArchive, this, &UnCompressPage::sigDeleteArchiveFiles);
    connect(m_fileviewer, &fileViewer::sigFileAutoCompressToArchive, this, &UnCompressPage::sigAddArchiveFiles);
}

void UnCompressPage::oneCompressPress()
{
    QFileInfo m_fileDestinationPath(m_pathstr);
    bool m_permission = (m_fileDestinationPath.isWritable() && m_fileDestinationPath.isExecutable());

    if (!m_permission) {
        showWarningDialog(tr("You do not have permission to save files here, please change and retry"));
        return;
    } else {
        emit sigDecompressPress(m_pathstr);
    }
}

void UnCompressPage::setModel(ArchiveSortFilterModel *model)
{
    m_model = model;
    m_fileviewer->setDecompressModel(m_model);
}

void UnCompressPage::onPathButoonClicked()
{
    DFileDialog dialog(this);
    dialog.setAcceptMode(DFileDialog::AcceptOpen);
    dialog.setFileMode(DFileDialog::Directory);
    dialog.setWindowTitle(tr("Find directory"));
    dialog.setDirectory(m_pathstr);

    const int mode = dialog.exec();

    if (mode != QDialog::Accepted) {
        return;
    }

    QList<QUrl> pathlist = dialog.selectedUrls();

    QString curpath = pathlist.at(0).toLocalFile();
    m_extractpath->setText(tr("Extract to:") + curpath);
    m_pathstr = curpath;
}

void UnCompressPage::setdefaultpath(QString path)
{
    m_pathstr = path;
    m_extractpath->setText(tr("Extract to:") + m_pathstr);
}

void UnCompressPage::SetDefaultFile(QFileInfo info)
{
    m_info = info;
}

int UnCompressPage::getFileCount()
{
    return m_fileviewer->getFileCount();
}

int UnCompressPage::showWarningDialog(const QString &msg)
{
    DDialog *dialog = new DDialog(this);
    QPixmap pixmap = Utils::renderSVG(":/icons/deepin/builtin/icons/compress_warning_32px.svg", QSize(32, 32));
    dialog->setIcon(pixmap);
//    dialog->setMessage(msg);
    dialog->addSpacing(32);
    dialog->addButton(tr("OK"));
    dialog->setMinimumSize(380, 140);
    DLabel *pContent = new DLabel(msg, dialog);
    pContent->setAlignment(Qt::AlignmentFlag::AlignHCenter);
    DPalette pa;
    pa = DApplicationHelper::instance()->palette(pContent);
    pa.setBrush(DPalette::Text, pa.color(DPalette::ButtonText));
    DFontSizeManager::instance()->bind(pContent, DFontSizeManager::T6, QFont::Medium);
    pContent->setMinimumWidth(this->width());
    pContent->move(dialog->width() / 2 - pContent->width() / 2, dialog->height() / 2 - pContent->height() / 2 - 10);
    int res = dialog->exec();
    delete dialog;

    return res;
}

void UnCompressPage::slotCompressedAddFile()
{
    DFileDialog dialog(this);
    dialog.setAcceptMode(DFileDialog::AcceptOpen);
    dialog.setFileMode(DFileDialog::ExistingFiles);
    dialog.setAllowMixedSelection(true);

    const int mode = dialog.exec();;

    // if click cancel button or close button.
    if (mode != QDialog::Accepted) {
        return;
    }

    emit sigAutoCompress(m_info.filePath(), dialog.selectedFiles());
}

QString UnCompressPage::getDecompressPath()
{
    return m_pathstr;
}

void UnCompressPage::onextractfilesSlot(QVector<Archive::Entry *> fileList, EXTRACT_TYPE type, QString path)
{
    if (fileList.count() == 0) {
        return;
    }

    if (EXTRACT_TO == type) {
        DFileDialog dialog(this);
        dialog.setAcceptMode(DFileDialog::AcceptOpen);
        dialog.setFileMode(DFileDialog::Directory);
        dialog.setDirectory(m_pathstr);

        const int mode = dialog.exec();

        if (mode != QDialog::Accepted) {
            return;
        }

        QList<QUrl> pathlist = dialog.selectedUrls();
        QString curpath = pathlist.at(0).toLocalFile();

        emit sigextractfiles(fileList, curpath, type);
    } else if (EXTRACT_DRAG == type) {
        emit sigextractfiles(fileList, path, type);
    } else if (EXTRACT_TEMP == type) {
        QString tmppath = DStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QDir::separator() + "tempfiles";
        QDir dir(tmppath);
        if (!dir.exists()) {
            dir.mkdir(tmppath);
        }
        emit sigextractfiles(fileList, tmppath, type);
    } else if (EXTRACT_TEMP_CHOOSE_OPEN == type) {
        QString tmppath = DStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QDir::separator() + "tempfiles";
        QDir dir(tmppath);
        if (!dir.exists()) {
            dir.mkdir(tmppath);
        }
        emit sigextractfiles(fileList, tmppath, type);
    }
}

void UnCompressPage::onRefreshFilelist(const QStringList &filelist)
{
    m_filelist = filelist;
    m_fileviewer->setFileList(m_filelist);

    emit sigRefreshFileList(m_filelist);

    if (m_filelist.size() == 0) {
        emit sigFilelistIsEmpty();
    }
}

void UnCompressPage::onextractfilesOpenSlot(const QVector<Archive::Entry *> &fileList, const QString &programma)
{
    emit sigOpenExtractFile(fileList, programma);
}

void UnCompressPage::onAutoCompress(const QStringList &path)
{
    emit sigAutoCompress(m_info.filePath(), path);
}
