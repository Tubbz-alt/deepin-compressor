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

#include "mainwindow.h"
#include "compressorapplication.h"
#include "utils.h"
#include "monitorInterface.h"
#include "environments.h"
#include "accessible.h"
#include "DebugTimeManager.h"

#include <DWidgetUtil>
#include <DLog>
#include <DApplicationSettings>

#include <QAccessible>
#include <QCommandLineParser>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    PERF_PRINT_BEGIN("POINT-01", ""); //启动计时
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    // load dtk xcb plugin.
    // 在dde桌面环境中，不调用次函数也会默认使用dxcb插件，且使用此函数强制指定加载dxcb会导致在和wayland环境下不兼容
    // 综上所述，此函数已经废弃
    //DApplication::loadDXcbPlugin();

    // init Dtk application's attrubites.
    CompressorApplication app(argc, argv);

    if (Utils::judgeKlu()) {
        qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
    }

    // add command line parser to app.
    QCommandLineParser parser;
    parser.setApplicationDescription("Deepin Compressor.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("filename", "File path.", "file [file..]");
    parser.process(app);

    const QStringList fileList = parser.positionalArguments();
    QStringList newfilelist;
    foreach (QString file, fileList) {
        if (file.contains("file://")) {
            file.remove("file://");
        }

        newfilelist.append(file);
    }

    QStringList multilist;
    if (newfilelist.count() > 0 && ((newfilelist.last() == QStringLiteral("extract_here_split_multi") || newfilelist.last() == QStringLiteral("extract_split_multi")))) {
        multilist.append(newfilelist.at(0));
        multilist.append(newfilelist.last().remove("_multi"));
        newfilelist = multilist;
    }

    bool isMutlWindows = false;
    if (argc >= 3 && !QString(argv[2]).contains(HEADBUS)) {
        QString lastStr = argv[argc - 1];
        QSet<QString> fstList;
        if (lastStr != "extract_here" && lastStr != "extract_here_multi" && lastStr != "extract" && lastStr != "extract_multi"
                && lastStr != "compress" && lastStr != "compress_to_zip" && lastStr != "compress_to_7z" && lastStr != "extract_here_split"
                && lastStr != "extract_split" && lastStr != "extract_here_split_multi" && lastStr != "extract_split_multi"
                && lastStr != "extract_mkdir" && lastStr != "extract_mkdir_multi" && lastStr != "extract_mkdir_split" && lastStr != "extract_mkdir_split_multi") {
            isMutlWindows = true;

            for (int i = 1; i < argc; i++) {
                if (QString(argv[i]).contains(".7z.")) {
                    QString tempfile = QString(argv[i]).left(QString(argv[i]).length() - 3);
                    bool isFind = false;
                    foreach (QString file, fstList) {
                        if (file.contains(tempfile)) {
                            isFind = true;
                            break;
                        }
                    }

                    if (!isFind) {
                        fstList.insert(QString(argv[i]));
                    }
                } else {
                    if (fstList.find(argv[i]) == fstList.end()) {
                        fstList.insert(QString(argv[i]));
                    }
                }
            }

            foreach (QString file, fstList) {
                QProcess p;
                QString command = "xdg-open";
                QStringList args;
                args.append(file);
                p.start(command, args);
                p.waitForFinished();
            }

            app.exit();
            return 0;
        }
    }

    QDBusConnection bus = QDBusConnection::sessionBus();
    bool busRegistered = bus.registerService("com.archive.mainwindow.monitor");

    if (argc == 1 || argc == 2 || (!isMutlWindows && argc >= 3)) {
        if (busRegistered == false) {
            com::archive::mainwindow::monitor monitor("com.archive.mainwindow.monitor", HEADBUS, QDBusConnection::sessionBus());
            if (monitor.isValid()) {
                QDBusPendingReply<bool> reply = monitor.createSubWindow(newfilelist);
                reply.waitForFinished();
                if (reply.isValid()) {
                    bool isClosed = reply.value();
                    if (isClosed) {
                        app.exit();
                        return 0;
                    }
                }
            }
        }
    }

    app.setOrganizationName("deepin");
    app.setApplicationName("deepin-compressor");
    app.loadTranslator();
    app.setApplicationVersion(DApplication::buildVersion(QDate::currentDate().toString("yyyyMMdd")));
    app.setApplicationAcknowledgementPage("https://www.deepin.org/original/deepin-compressor/");
    app.setProductIcon(QIcon::fromTheme("deepin-compressor"));
    app.setApplicationVersion(VERSION);
    app.setProductName(DApplication::translate("Main", "Archive Manager"));
    app.setApplicationDisplayName(DApplication::translate("Main", "Archive Manager"));
    app.setApplicationDescription(DApplication::translate("Main", "Archive Manager is a fast and lightweight application for creating and extracting archives."));
    DApplicationSettings settings(&app);

    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

    // 打印输入参数
    // for (int i = 0; i < argc; ++i) {
    //     qDebug() << QString("argv[%1]:").arg(i) << argv[i];
    // }

    //Accessibility自动化
#ifdef ACCESSIBILITY_ON
    QAccessible::installFactory(accessibleFactory);
#endif

    QIcon appIcon = QIcon::fromTheme("deepin-compressor");

    if (appIcon.isNull()) {
        appIcon = QIcon(":assets/icons/deepin/builtin/icons/deepin-compressor.svg");
    }

    app.setProductIcon(appIcon);
    app.setWindowIcon(appIcon);

    MainWindow w;
    app.setMainWindow(&w);

    //判断目标文件是否合法
    if (argc >= 2 && !w.checkSettings(argv[1])) {
        app.exit();
        return 0;
    } else {
        w.bindAdapter();

        if (busRegistered == true) {
            // init modules.
            if (app.setSingleInstance("deepin-compressor")) {
                Dtk::Widget::moveToCenter(&w);
            }

            QString strWId = QString::number(w.winId());
            bus.registerObject(HEADBUS, &w);

            QObject::connect(&w, &MainWindow::sigquitApp, &app, &DApplication::quit);

            // handle command line parser.
            // 右键时show()函数在MainWindow的onRightMenuSelected中调用
            if (!newfilelist.isEmpty()) {
                QMetaObject::invokeMethod(&w, "onRightMenuSelected", Qt::DirectConnection, Q_ARG(QStringList, newfilelist));
            } else {
                w.show();
                PERF_PRINT_END("POINT-01");
            }
        }
    }

    return app.exec();
}
