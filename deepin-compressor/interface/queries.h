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
#ifndef QUERIES_H
#define QUERIES_H

#include <DApplicationHelper>
#include <DCheckBox>

#include <QString>
#include <QHash>
#include <QWaitCondition>
#include <QMutex>
#include <QVariant>

#define TITLE_FIXED_HEIGHT 50               // 标题栏高度
DWIDGET_USE_NAMESPACE

enum RenameDialog_Result {
    Result_Cancel = 0,

    Result_Skip = 1,
    Result_AutoSkip = 2,
    Result_Overwrite = 3,
    Result_OverwriteAll = 4,
    Result_Resume = 5,
    Result_ResumeAll = 6,
    Result_AutoRename = 7,
    Result_Retry = 8,
    Result_Rename = 9,

    // @deprecated since 5.0, use the RenameDialog_Option enum values
    R_CANCEL = Result_Cancel,
    R_RENAME = Result_Rename,
    R_SKIP = Result_Skip,
    R_AUTO_SKIP = Result_AutoSkip,
    R_OVERWRITE = Result_Overwrite,
    R_OVERWRITE_ALL = Result_OverwriteAll,
    R_RESUME = Result_Resume,
    R_RESUME_ALL = Result_ResumeAll,
    R_AUTO_RENAME = Result_AutoRename,
    R_RETRY = Result_Retry,

    S_CANCEL = Result_Cancel,
    S_SKIP = Result_Skip,
    S_AUTO_SKIP = Result_AutoSkip,
    S_RETRY = Result_Retry
};

typedef QHash<QString, QVariant> QueryData;

class  Query
{
public:

    static QString toShortString(QString strSrc, int limitCounts = 16, int left = 8);

    virtual void execute() = 0;
    void waitForResponse();
    void setResponse(const QVariant &response);
    QVariant response() const;

    int execDialog();
protected:
    Query();
    virtual ~Query() {}



    QueryData m_data;

private:
    QWaitCondition m_responseCondition;
    QMutex m_responseMutex;
};

/* *****************************************************************
 * Used to query the user if an existing file should be overwritten.
 * *****************************************************************
 */
class  OverwriteQuery : public Query
{
public:
    explicit OverwriteQuery(const QString &filename);
    void execute() override;
    int  getExecuteReturn();
    bool responseCancelled();
    bool responseOverwriteAll();
    bool responseOverwrite();
    bool responseRename();
    bool responseSkip();
    bool responseAutoSkip();
    QString newFilename();

    void setNoRenameMode(bool enableNoRenameMode);
    bool noRenameMode();
    void setMultiMode(bool enableMultiMode);
    bool multiMode();

    void colorRoleChange(QWidget *widget, DPalette::ColorRole ct, double alphaF);
    void colorTypeChange(QWidget *widget, DPalette::ColorType ct, double alphaF);

    bool applyAll();

private:
    bool m_noRenameMode;
    bool m_multiMode;
    int ret;
    bool m_bApplyAll;
};

/* **************************************
 * Used to query the user for a password.
 * **************************************
 */
class PasswordNeededQuery : public Query
{
public:
    explicit PasswordNeededQuery(const QString &archiveFilename, bool incorrectTryAgain = false);
    void execute() override;

    bool responseCancelled();
    QString password();
};

/* **************************************
 * Used to query the user for a password.
 * **************************************
 */
class WrongPasswordQuery : public Query
{
public:
    explicit WrongPasswordQuery(const QString &archiveFilename, bool incorrectTryAgain = false);
    void execute() override;

    bool responseCancelled();
    QString password();
};

/* *************************************************************
 * Used to query the user if a corrupt archive should be loaded.
 * *************************************************************
 */
class LoadCorruptQuery : public Query
{
public:
    explicit LoadCorruptQuery(const QString &archiveFilename);
    void execute() override;

    bool responseYes();
};

class ContinueExtractionQuery : public Query
{
public:
    explicit ContinueExtractionQuery(const QString &error, const QString &archiveEntry);
    void execute() override;

    bool responseCancelled();
    bool dontAskAgain();
private:
    QCheckBox m_chkDontAskAgain;
};

#endif /* ifndef QUERIES_H */
