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

#include "batchextract.h"
#include "jobs.h"
#include "queries.h"
#include "utils.h"
#include "settingdialog.h"
#include "mimetypes.h"

#include <QDir>
#include <QFileInfo>
#include <QPointer>
#include <QTimer>

BatchExtract::BatchExtract(QObject *parent)
    : BatchJobs(parent),
      m_autoSubfolder(false),
      m_preservePaths(true),
      m_openDestinationAfterExtraction(false)
{
    mType = Job::ENUM_JOBTYPE::BATCHEXTRACTJOB;

    setCapabilities(KJob::Killable);

    connect(this, &KJob::result, this, &BatchExtract::showFailedFiles);
}

void BatchExtract::addExtraction(const QUrl &url)
{
    m_numOfExtracting = 0;
    m_lastPercent = 0;

    m_settingDialog = new SettingDialog;
    m_pSettingInfo = new Settings_Extract_Info;

    QString destination = destinationFolder();
    QVector<Archive::Entry *> files;
    ExtractionOptions options;
//    options.setRightMenuExtractHere(m_isExtractHere);
    options.setAutoCreatDir(m_settingDialog->isAutoCreatDir());
    options.setBatchExtract(true);

    QFileInfo fi(url.toLocalFile());
    QString userDestination = fi.path();
    QString detectedSubfolder = "";

    if (m_settingDialog->isAutoCreatDir()) {   //自动创建文件夹
        detectedSubfolder = fi.completeBaseName();
        if (fi.filePath().contains(".tar.")) {
            detectedSubfolder = detectedSubfolder.remove(".tar");
        } else if (fi.filePath().contains(".7z.")) {
            detectedSubfolder = detectedSubfolder.remove(".7z");
        } else if (fi.filePath().contains(".part01.rar")) {
            detectedSubfolder = detectedSubfolder.remove(".part01");
        } else if (fi.filePath().contains(".part1.rar")) {
            detectedSubfolder = detectedSubfolder.remove(".part1");
        } else if (fi.filePath().contains(".zip.")) {
            detectedSubfolder = detectedSubfolder.remove(".zip");
        }

        m_pSettingInfo->str_CreateFolder = detectedSubfolder;
        if (!userDestination.endsWith(QDir::separator())) {
            userDestination.append(QDir::separator());
        }

        destination = destination + QDir::separator() + detectedSubfolder + QDir::separator();
        QDir dir(destination);
        if (!dir.exists()) {
            dir.mkpath(destination);
        }
    } else {
//        destination = userDestination;
        m_pSettingInfo->str_CreateFolder = detectedSubfolder;
    }
    QString transFile = url.toLocalFile();
    SpecialFileAttributes attributes;
    transSplitFileName(transFile, &attributes);     // 对文件名进行转换（分卷处理）

    QString fixedMimetype = determineMimeType(url.toLocalFile()).name();
    ReadOnlyArchiveInterface *pIface = Archive::createInterface(url.toLocalFile(), fixedMimetype, true, &attributes);
    ExtractJob *job = new ExtractJob(files, destination, options, pIface);
    qDebug() << QString(QStringLiteral("Registering job from archive %1, to %2, preservePaths %3")).arg(url.toLocalFile(), destination, QString::number(preservePaths()));

    addSubjob(job);

    m_fileNames[job] = qMakePair(url.toLocalFile(), destination);

    connect(job, SIGNAL(percent(KJob *, ulong)), this, SLOT(forwardProgress(KJob *, ulong)));
    connect(job, SIGNAL(percentfilename(KJob *, const QString &)), this, SLOT(SlotProgressFile(KJob *, const QString &)));
    connect(job, &ExtractJob::userQuery, this, &BatchExtract::signalUserQuery);

//    QString destination = destinationFolder();

//    auto job = Archive::batchExtract(url.toLocalFile(), destination, autoSubfolder(), preservePaths());

//    qDebug() << QString(QStringLiteral("Registering job from archive %1, to %2, preservePaths %3")).arg(url.toLocalFile(), destination, QString::number(preservePaths()));

//    addSubjob(job);

//    m_fileNames[job] = qMakePair(url.toLocalFile(), destination);

//    connect(job, SIGNAL(percent(KJob *, ulong)),
//            this, SLOT(forwardProgress(KJob *, ulong)));
//    connect(job, &BatchExtractJob::userQuery,
//            this, &BatchExtract::signalUserQuery);
//    connect(job, SIGNAL(percentfilename(KJob *, const QString &)),
//            this, SLOT(SlotProgressFile(KJob *, const QString &)));
//    connect(job, &BatchExtractJob::signeedpassword,
//    this, [ = ] {
//        qDebug() << "need password";
//    });
}

void BatchExtract::SlotProgressFile(KJob *job, const QString &name)
{
    emit batchFilenameProgress(job, name);
}

void BatchExtract::transSplitFileName(QString &fileName, SpecialFileAttributes *attributes)
{
    if (fileName.contains(".7z.")) {
        QRegExp reg("^([\\s\\S]*.)[0-9]{3}$"); // QRegExp reg("[*.]part\\d+.rar$"); //rar分卷不匹配

        if (reg.exactMatch(fileName) == false) {
            return;
        }

        QFileInfo fi(reg.cap(1) + "001");

        if (fi.exists() == true) {
            fileName = reg.cap(1) + "001";
        }
    } else if (fileName.contains(".part") && fileName.endsWith(".rar")) {
        int x = fileName.lastIndexOf("part");
        int y = fileName.lastIndexOf(".");

        if ((y - x) > 5) {
            fileName.replace(x, y - x, "part01");
        } else {
            fileName.replace(x, y - x, "part1");
        }
    } else if (fileName.contains(".zip.")) {
        QRegExp reg("^([\\s\\S]*.)[0-9]{3}$");
        if (reg.exactMatch(fileName) == false) {
            return;
        }
        QFileInfo fi(reg.cap(1) + "001");
        if (fi.exists() == true) {
            fileName = reg.cap(1) + "001";
        }

        if (nullptr != attributes) {
            attributes->b_isZipSplit = true;
        }
    } else if (fileName.endsWith(".zip")) {
        /**
         * 例如123.zip文件，检测123.z01文件是否存在
         * 如果存在，则认定123.zip是分卷包
         */
        QFileInfo tmp(fileName.left(fileName.length() - 2) + "01");
        if (tmp.exists()) {
            if (nullptr != attributes) {
                attributes->b_isZipSplit = true;
            }
        }
    }
}

bool BatchExtract::doKill()
{
    if (subjobs().isEmpty()) {
        return false;
    }

    return subjobs().first()->kill();
}

//void BatchExtract::slotUserQuery(Query *query)
//{
//    query->execute();
//}

bool BatchExtract::autoSubfolder() const
{
    return m_autoSubfolder;
}

void BatchExtract::setAutoSubfolder(bool value)
{
    m_autoSubfolder = value;
}

void BatchExtract::start()
{
    QTimer::singleShot(0, this, &BatchExtract::slotStartJob);
}

void BatchExtract::slotStartJob()
{
    if (m_inputs.isEmpty()) {
        emitResult();
        return;
    }

    for (const auto &url : qAsConst(m_inputs)) {
        addExtraction(url);
    }

    m_initialJobCount = subjobs().size();

    qDebug() << "Starting first job";

    m_job = subjobs().at(0);

    connect(m_job, &Job::sigBatchExtractJobWrongPsd, this, [&]() {
        Q_ASSERT(m_job);
        m_job->start(); //批量解压密码错误时，重新走解压流程
    });

    m_job->start();
}

void BatchExtract::showFailedFiles()
{
    if (!m_failedFiles.isEmpty()) {
//        KMessageBox::informationList(nullptr, i18n("The following files could not be extracted:"), m_failedFiles);
    }
}

void BatchExtract::slotResult(KJob *job)
{
    //批量解压，关闭密码输入框跳过当前压缩包
    if (job->error() && job->error() != KJob::CancelError && job->error() != KJob::NopasswordError
            && job->error() != KJob::UserSkiped) {
        qDebug() << "There was en error:" << job->error() << ", errorText:" << job->errorString();

        QString curfile = m_fileNames[subjobs().at(0)].first;
        qDebug() << "Fail curfilename" << curfile;
        QFileInfo file(curfile);
//        while (hasSubjobs()) {
//            removeSubjob(job);
//        }

        removeSubjob(job);
        emit sendFailFile(file.fileName());

        return;
    }

    removeSubjob(job);

    if (!hasSubjobs()) {
        qDebug() << "Finished, emitting the result";
        emitResult();
    } else {
        qDebug() << "Starting the next job";
        m_job = subjobs().at(0);
        m_job->start();

        connect(m_job, &Job::sigBatchExtractJobWrongPsd, this, [&]() {
            Q_ASSERT(m_job);
            m_job->start(); //批量解压密码错误时，重新走解压流程
        });
        QString curfile = m_fileNames[subjobs().at(0)].first;
        qDebug() << "Send curfilename" << curfile;
        QFileInfo file(curfile);
        emit sendCurFile(file.fileName());
    }
}

void BatchExtract::forwardProgress(KJob *job, unsigned long percent)
{
    Q_UNUSED(job)

    QString path = m_inputs.at(m_numOfExtracting).toString().remove("file://");
    QFile file(path);
    qint64 perFileSize = 0;
    if (file.exists()) {
        perFileSize = file.size();
    }

    qDebug() << percent;

//    auto jobPart = static_cast<ulong>(100 / m_initialJobCount);
//    auto remainingJobs = static_cast<ulong>(m_initialJobCount - subjobs().size());

    ulong actualPercent = ((percent * perFileSize) / m_batchTotalSize) + m_lastPercent;
    emit batchProgress(job, actualPercent);

    if (percent > 0 && percent % 100 == 0) {
        m_lastPercent += ((100 * perFileSize) / m_batchTotalSize);
        m_numOfExtracting++;
    }
}

void BatchExtract::addInput(const QUrl &url)
{
    qDebug() << "Adding archive" << url.toLocalFile();

    if (!QFileInfo::exists(url.toLocalFile())) {
        m_failedFiles.append(url.fileName());
        return;
    }

    m_inputs.append(url);
}

bool BatchExtract::openDestinationAfterExtraction() const
{
    return m_openDestinationAfterExtraction;
}

bool BatchExtract::preservePaths() const
{
    return m_preservePaths;
}

QString BatchExtract::destinationFolder() const
{
    qDebug() << m_destinationFolder;
    if (m_destinationFolder.isEmpty()) {
        return QDir::currentPath();
    } else {
        return m_destinationFolder;
    }
}

void BatchExtract::setDestinationFolder(const QString &folder)
{
    if (QFileInfo(folder).isDir()) {
        m_destinationFolder = folder;
    }
}

void BatchExtract::setOpenDestinationAfterExtraction(bool value)
{
    m_openDestinationAfterExtraction = value;
}

void BatchExtract::setPreservePaths(bool value)
{
    m_preservePaths = value;
}

void BatchExtract::setBatchTotalSize(qint64 size)
{
    m_batchTotalSize = size;
}

bool BatchExtract::showExtractDialog()
{
    return true;
}
