/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *               2016 ~ 2018 dragondjf
 *
 * Author:     dragondjf<dingjiangfeng@deepin.com>
 *
 * Maintainer: dragondjf<dingjiangfeng@deepin.com>
 *             zccrs<zhangjide@deepin.com>
 *             Tangtong<tangtong@deepin.com>
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

#ifndef OPENWITHDIALOG_H
#define OPENWITHDIALOG_H

#include "basedialog.h"

#include <dflowlayout.h>
#include <QCommandLinkButton>

#include <QObject>
#include <QUrl>
#include <QMimeType>
#include <QProcess>


#include "durl.h"
#include "singleton.h"
#include "dmimedatabase.h"
#include "mimesappsmanager.h"
#include "kprocess.h"

DWIDGET_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QPushButton;
class QScrollArea;
class QCheckBox;
QT_END_NAMESPACE

enum ShowType {
    SelApp = 0,
    OpenApp
};

class OpenWithDialogListItem;
class OpenWithDialog : public BaseDialog
{
    Q_OBJECT
public:
    explicit OpenWithDialog(const DUrl &url, QWidget *parent = nullptr);

    static QList<QAction *> addMenuOpenAction(const QString &fileName, DMenu *menu);
    static void chooseOpen(const QString &programma, const QString &fileName);
    static QString includePercentFile(const QString &file);

    void SetShowType(ShowType type);
    QString AppDisplayName();

    ~OpenWithDialog() override;

public slots:
    void openFileByApp();

protected:
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void initUI();
    void initConnect();
    void initData();
    void checkItem(OpenWithDialogListItem *item);
    void useOtherApplication();
    OpenWithDialogListItem *createItem(const QIcon &icon, const QString &name, const QString &filePath);

    QScrollArea *m_scrollArea = nullptr;
    DFlowLayout *m_recommandLayout = nullptr;
    DFlowLayout *m_otherLayout = nullptr;

    QCommandLinkButton *m_openFileChooseButton = nullptr;
    QCheckBox *m_setToDefaultCheckBox = nullptr;
    QPushButton *m_cancelButton = nullptr;
    QPushButton *m_chooseButton = nullptr;
    DUrl m_url;
    QMimeType m_mimeType;

    OpenWithDialogListItem *m_checkedItem = nullptr;

    ShowType m_showType;
    QString m_strAppDisplayName;
};

#endif // OPENWITHDIALOG_H
