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
#ifndef ENCRYPTIONPAGE_H
#define ENCRYPTIONPAGE_H

#include <DWidget>
#include <DPushButton>
#include <DLabel>
#include <DPasswordEdit>

DWIDGET_USE_NAMESPACE

class EncryptionPage: public DWidget
{
    Q_OBJECT
public:
    EncryptionPage(QWidget *parent = nullptr);
    void InitUI();
    void InitConnection();

    void setPassowrdFocus();
    void resetPage();
private:
signals:
    void sigExtractPassword(QString password);

public slots:
    void nextbuttonClicked();
    void wrongPassWordSlot();
    void onPasswordChanged();

private:
    DPushButton *m_nextbutton = nullptr;
    DPasswordEdit *m_password = nullptr;

    bool m_inputflag = false;
    bool pwdCheckDown = true;
};

#endif // ENCRYPTIONPAGE_H
