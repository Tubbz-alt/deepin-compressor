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

#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "filewatcher.h"

#include <DWidget>
#include <DCommandLinkButton>
#include <DSpinner>
#include <DLabel>
//#include <DFileDialog>

#include <QVBoxLayout>
#include <QSettings>

DWIDGET_USE_NAMESPACE

class HomePage : public DWidget
{
    Q_OBJECT

public:
    HomePage(QWidget *parent = nullptr);

    void setIconPixmap(bool isLoaded);
    void spinnerStart(QObject *pWnd = nullptr, pMember_callback func = nullptr);
    void spinnerStop();

    void resizeEvent(QResizeEvent *event) override;

signals:
    void fileSelected(const QStringList files) const;

public slots:
    void themeChanged();
    void slotSpinnerStart(bool result);
private:
    void onChooseBtnClicked();

private:
    QVBoxLayout *m_layout;
    QPixmap m_unloadPixmap;
    QPixmap m_loadedPixmap;
    DLabel *m_iconLabel;
    DLabel *m_tipsLabel;
    DLabel *m_splitLine;
    DCommandLinkButton *m_chooseBtn;
    QSettings *m_settings;
    DSpinner *m_spinner;
    TimerWatcher *m_pWatcher = nullptr;
};

#endif
