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

#include "utils.h"
#include "mimetypes.h"

#include <DStandardPaths>

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QDebug>
#include <QFileInfo>
#include <QFontInfo>
#include <QMimeType>
#include <QApplication>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QImageReader>
#include <QPixmap>
#include <QTextCodec>
#include <QRegularExpression>

#include <KEncodingProber>

DCORE_USE_NAMESPACE

QStringList Utils::m_associtionlist = QStringList() << "file_association.file_association_type.x-7z-compressed"
                                      << "file_association.file_association_type.x-archive"
                                      << "file_association.file_association_type.x-bcpio"
                                      << "file_association.file_association_type.x-bzip"
                                      << "file_association.file_association_type.x-cpio"
                                      << "file_association.file_association_type.x-cpio-compressed"
                                      << "file_association.file_association_type.vnd.debian.binary-package"
                                      << "file_association.file_association_type.gzip"
                                      << "file_association.file_association_type.x-java-archive"
                                      << "file_association.file_association_type.x-lzma"
                                      << "file_association.file_association_type.vnd.ms-cab-compressed"
                                      << "file_association.file_association_type.vnd.rar"
                                      << "file_association.file_association_type.x-rpm"
                                      << "file_association.file_association_type.x-sv4cpio"
                                      << "file_association.file_association_type.x-sv4crc"
                                      << "file_association.file_association_type.x-tar"
                                      << "file_association.file_association_type.x-bzip-compressed-tar"
                                      << "file_association.file_association_type.x-compressed-tar"
                                      << "file_association.file_association_type.x-lzip-compressed-tar"
                                      << "file_association.file_association_type.x-lzma-compressed-tar"
                                      << "file_association.file_association_type.x-tzo"
                                      << "file_association.file_association_type.x-xz-compressed-tar"
                                      << "file_association.file_association_type.x-tarz"
                                      << "file_association.file_association_type.x-xar"
                                      << "file_association.file_association_type.x-xz"
                                      << "file_association.file_association_type.zip"
                                      << "file_association.file_association_type.x-cd-image"
                                      << "file_association.file_association_type.x-iso9660-appimage"
                                      << "file_association.file_association_type.x-source-rpm";

Utils::Utils(QObject *parent)
    : QObject(parent)
{
}

Utils::~Utils()
{
}

QString Utils::getConfigPath()
{
    QDir dir(QDir(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first())
             .filePath(qApp->organizationName()));

    return dir.filePath(qApp->applicationName());
}


QString Utils::suffixList()
{
    return QString("");
}

QPixmap Utils::renderSVG(const QString &filePath, const QSize &size)
{
    QImageReader reader;
    QPixmap pixmap;

    reader.setFileName(filePath);

    if (reader.canRead()) {
        const qreal ratio = qApp->devicePixelRatio();
        reader.setScaledSize(size * ratio);
        pixmap = QPixmap::fromImage(reader.read());
        pixmap.setDevicePixelRatio(ratio);
    } else {
        pixmap.load(filePath);
    }

    return pixmap;
}

bool Utils::isCompressed_file(const QString &filePath)
{
    QMimeType mimeType = determineMimeType(filePath);
    qDebug() << mimeType;
    QString mime;
    if (mimeType.name().contains("application/"))
        mime = mimeType.name().remove("application/");
    // = judgeFileMime(filePath);         // 根据文件名（后缀）判断文件类型

    bool ret = false;

    if (mime.size() > 0) {
        ret = existMimeType(mime);
    } else {
        ret = false;
    }

    if (filePath.endsWith(".deb")) {    // 对deb文件识别为普通文件
        ret = false;
    }

    if (filePath.endsWith(".crx")) {    // 对crx文件识别为压缩包
        ret = true;
    }

    return ret;

//    if (filetype.compare("application/x-7z-compressedr") && filetype.compare("application/zip") && filetype.compare("application/vnd.rar") && filetype.compare("application/x-rar")
//            && filetype.compare("application/x-java-archive") && filetype.compare("application/x-cd-image")
//            && filetype.compare("application/x-bcpio") && filetype.compare("application/x-cpio") && filetype.compare("application/x-cpio-compressed") && filetype.compare("application/x-sv4cpio")
//            && filetype.compare("application/x-sv4crc") && filetype.compare("application/x-rpm") && filetype.compare("application/x-source-rpm")
//            && filetype.compare("application/vnd.ms-cab-compressed") && filetype.compare("application/x-xar") && filetype.compare("application/x-iso9660-appimage")
//            && filetype.compare("application/x-tarz") && filetype.compare("application/x-tar") && filetype.compare("application/x-compressed-tar") && filetype.compare("application/x-bzip-compressed-tar")
//            && filetype.compare("application/x-xz-compressed-tar") && filetype.compare("application/x-lzma-compressed-tar") && filetype.compare("application/x-lzip-compressed-tar")
//            && filetype.compare("application/x-tzo") && filetype.compare("application/x-lrzip-compressed-tar") && filetype.compare("application/x-lz4-compressed-tar")
//            && filetype.compare("application/x-zstd-compressed-tar") && filetype.compare("application/x-bzip") && filetype.compare("application/gzip") && filetype.compare("application/x-lzma")
//            && filetype.compare("application/x-xz") && filetype.compare("application/zip")) {
//        ret = false;
//    }

//    QFileInfo file(filePath);
//    qDebug() << file.suffix();
//    if (true == ret) {
//        if (file.suffix() == "pptx" || file.suffix() == "ppt" || file.suffix() == "xls" ||
//                file.suffix() == "xlsx" || file.suffix() == "doc" || file.suffix() == "docx"/* || file.suffix() == "xmind"*/) {
//            ret = false;
//        }
//    } else if (filePath.contains(".7z.")) {
//        ret = true;
//    }
//    qDebug() << ret;
//    return ret;
}

QString Utils::humanReadableSize(const qint64 &size, int precision)
{
    if (size == 0) {
        return "-";
    }

    double sizeAsDouble = size;
    static QStringList measures;
    if (measures.isEmpty())
        measures << QCoreApplication::translate("QInstaller", "B")
                 << QCoreApplication::translate("QInstaller", "KB")
                 << QCoreApplication::translate("QInstaller", "MB")
                 << QCoreApplication::translate("QInstaller", "GB")
                 << QCoreApplication::translate("QInstaller", "TB")
                 << QCoreApplication::translate("QInstaller", "PB")
                 << QCoreApplication::translate("QInstaller", "EB")
                 << QCoreApplication::translate("QInstaller", "ZB")
                 << QCoreApplication::translate("QInstaller", "YB");
    QStringListIterator it(measures);
    QString measure(it.next());
    while (sizeAsDouble >= 1024.0 && it.hasNext()) {
        measure = it.next();
        sizeAsDouble /= 1024.0;
    }
    return QString::fromLatin1("%1 %2").arg(sizeAsDouble, 0, 'f', precision).arg(measure);
}

qint64 Utils::humanReadableToSize(const QString &size)
{
    QString sizeAsStr = size;
    static QStringList measures;
    if (measures.isEmpty())
        measures << (" B")
                 << (" KB")
                 << (" MB")
                 << (" GB")
                 << (" TB")
                 << (" PB")
                 << (" EB")
                 << (" ZB")
                 << (" YB");

    int loop = 0;
    foreach (QString suffix, measures) {
        if (sizeAsStr.contains(suffix)) {
            sizeAsStr.remove(suffix);
            break;
        }
        loop++;
    }

    if (loop > measures.count()) {
        return 0;
    }

    float intsize = sizeAsStr.toFloat();

    for (int i = 0; i < loop; i++) {
        intsize *= 1024.0;
    }

    return (qint64)intsize;
}

static float codecConfidenceForData(const QTextCodec *codec, const QByteArray &data, const QLocale::Country &country)
{
    qreal hep_count = 0;
    int non_base_latin_count = 0;
    qreal unidentification_count = 0;
    int replacement_count = 0;

    QTextDecoder decoder(codec);
    const QString &unicode_data = decoder.toUnicode(data);

    for (int i = 0; i < unicode_data.size(); ++i) {
        const QChar &ch = unicode_data.at(i);

        if (ch.unicode() > 0x7f)
            ++non_base_latin_count;

        switch (ch.script()) {
        case QChar::Script_Hiragana:
        case QChar::Script_Katakana:
            hep_count += (country == QLocale::Japan) ? 1.2 : 0.5;
            unidentification_count += (country == QLocale::Japan) ? 0 : 0.3;
            break;
        case QChar::Script_Han:
            hep_count += (country == QLocale::China) ? 1.2 : 0.5;
            unidentification_count += (country == QLocale::China) ? 0 : 0.3;
            break;
        case QChar::Script_Hangul:
            hep_count += (country == QLocale::NorthKorea) || (country == QLocale::SouthKorea) ? 1.2 : 0.5;
            unidentification_count += (country == QLocale::NorthKorea) || (country == QLocale::SouthKorea) ? 0 : 0.3;
            break;
        case QChar::Script_Cyrillic:
            hep_count += (country == QLocale::Russia) ? 1.2 : 0.5;
            unidentification_count += (country == QLocale::Russia) ? 0 : 0.3;
            break;
        case QChar::Script_Devanagari:
            hep_count += (country == QLocale::Nepal || country == QLocale::India) ? 1.2 : 0.5;
            unidentification_count += (country == QLocale::Nepal || country == QLocale::India) ? 0 : 0.3;
            break;
        default:
            // full-width character, emoji, 常用标点, 拉丁文补充1，天城文补充，CJK符号和标点符号（如：【】）
            if ((ch.unicode() >= 0xff00 && ch <= 0xffef)
                    || (ch.unicode() >= 0x2600 && ch.unicode() <= 0x27ff)
                    || (ch.unicode() >= 0x2000 && ch.unicode() <= 0x206f)
                    || (ch.unicode() >= 0x80 && ch.unicode() <= 0xff)
                    || (ch.unicode() >= 0xa8e0 && ch.unicode() <= 0xa8ff)
                    || (ch.unicode() >= 0x0900 && ch.unicode() <= 0x097f)
                    || (ch.unicode() >= 0x3000 && ch.unicode() <= 0x303f)) {
                ++hep_count;
            } else if (ch.isSurrogate() && ch.isHighSurrogate()) {
                ++i;

                if (i < unicode_data.size()) {
                    const QChar &next_ch = unicode_data.at(i);

                    if (!next_ch.isLowSurrogate()) {
                        --i;
                        break;
                    }

                    uint unicode = QChar::surrogateToUcs4(ch, next_ch);

                    // emoji
                    if (unicode >= 0x1f000 && unicode <= 0x1f6ff) {
                        hep_count += 2;
                    }
                }
            } else if (ch.unicode() == QChar::ReplacementCharacter) {
                ++replacement_count;
            } else if (ch.unicode() > 0x7f) {
                // 因为UTF-8编码的容错性很低，所以未识别的编码只需要判断是否为 QChar::ReplacementCharacter 就能排除
                if (codec->name() != "UTF-8")
                    ++unidentification_count;
            }
            break;
        }
    }

    float c = qreal(hep_count) / non_base_latin_count / 1.2;

    c -= qreal(replacement_count) / non_base_latin_count;
    c -= qreal(unidentification_count) / non_base_latin_count;

    return qMax(0.0f, c);
}

QByteArray Utils::detectEncode(const QByteArray &data, const QString &fileName)
{
    // Return local encoding if nothing in file.
    if (data.isEmpty()) {
        return QTextCodec::codecForLocale()->name();
    }

    if (QTextCodec *c = QTextCodec::codecForUtfText(data, nullptr)) {
        return c->name();
    }

    QMimeDatabase mime_database;
    const QMimeType &mime_type = fileName.isEmpty() ? mime_database.mimeTypeForData(data) : mime_database.mimeTypeForFileNameAndData(fileName, data);
    const QString &mimetype_name = mime_type.name();
    KEncodingProber::ProberType proberType = KEncodingProber::Universal;

    if (mimetype_name == QStringLiteral("application/xml")
            || mimetype_name == QStringLiteral("text/html")
            || mimetype_name == QStringLiteral("application/xhtml+xml")) {
        const QString &_data = QString::fromLatin1(data);
        QRegularExpression pattern("<\\bmeta.+\\bcharset=(?'charset'\\S+?)\\s*['\"/>]");

        pattern.setPatternOptions(QRegularExpression::DontCaptureOption | QRegularExpression::CaseInsensitiveOption);
        const QString &charset = pattern.match(_data, 0, QRegularExpression::PartialPreferFirstMatch,
                                               QRegularExpression::DontCheckSubjectStringMatchOption).captured("charset");

        if (!charset.isEmpty()) {
            return charset.toLatin1();
        }

        pattern.setPattern("<\\bmeta\\s+http-equiv=\"Content-Language\"\\s+content=\"(?'language'[a-zA-Z-]+)\"");

        const QString &language = pattern.match(_data, 0, QRegularExpression::PartialPreferFirstMatch,
                                                QRegularExpression::DontCheckSubjectStringMatchOption).captured("language");

        if (!language.isEmpty()) {
            QLocale l(language);

            switch (l.script()) {
            case QLocale::ArabicScript:
                proberType = KEncodingProber::Arabic;
                break;
            case QLocale::SimplifiedChineseScript:
                proberType = KEncodingProber::ChineseSimplified;
                break;
            case QLocale::TraditionalChineseScript:
                proberType = KEncodingProber::ChineseTraditional;
                break;
            case QLocale::CyrillicScript:
                proberType = KEncodingProber::Cyrillic;
                break;
            case QLocale::GreekScript:
                proberType = KEncodingProber::Greek;
                break;
            case QLocale::HebrewScript:
                proberType = KEncodingProber::Hebrew;
                break;
            case QLocale::JapaneseScript:
                proberType = KEncodingProber::Japanese;
                break;
            case QLocale::KoreanScript:
                proberType = KEncodingProber::Korean;
                break;
            case QLocale::ThaiScript:
                proberType = KEncodingProber::Thai;
                break;
            default:
                break;
            }
        }
    } else if (mimetype_name == "text/x-python") {
        QRegularExpression pattern("^#coding\\s*:\\s*(?'coding'\\S+)$");
        QTextStream stream(data);

        pattern.setPatternOptions(QRegularExpression::DontCaptureOption | QRegularExpression::CaseInsensitiveOption);
        stream.setCodec("latin1");

        while (!stream.atEnd()) {
            const QString &_data = stream.readLine();
            const QString &coding = pattern.match(_data, 0).captured("coding");

            if (!coding.isEmpty()) {
                return coding.toLatin1();
            }
        }
    }

    // for CJK
    const QList<QPair<KEncodingProber::ProberType, QLocale::Country>> fallback_list {
        {KEncodingProber::ChineseSimplified, QLocale::China},
        {KEncodingProber::ChineseTraditional, QLocale::China},
        {KEncodingProber::Japanese, QLocale::Japan},
        {KEncodingProber::Korean, QLocale::NorthKorea},
        {KEncodingProber::Cyrillic, QLocale::Russia},
        {KEncodingProber::Greek, QLocale::Greece},
        {proberType, QLocale::system().country()}
    };

    KEncodingProber prober(proberType);
    prober.feed(data);
    float pre_confidence = prober.confidence();
    QByteArray pre_encoding = prober.encoding();

    QTextCodec *def_codec = QTextCodec::codecForLocale();
    QByteArray encoding;
    float confidence = 0;

    for (auto i : fallback_list) {
        prober.setProberType(i.first);
        prober.feed(data);

        float prober_confidence = prober.confidence();
        QByteArray prober_encoding = prober.encoding();

        if (i.first != proberType && qFuzzyIsNull(prober_confidence)) {
            prober_confidence = pre_confidence;
            prober_encoding = pre_encoding;
        }

    confidence:
        if (QTextCodec *codec = QTextCodec::codecForName(prober_encoding)) {
            if (def_codec == codec)
                def_codec = nullptr;

            float c = codecConfidenceForData(codec, data, i.second);

            if (prober_confidence > 0.5) {
                c = c / 2 + prober_confidence / 2;
            } else {
                c = c / 3 * 2 + prober_confidence / 3;
            }

            if (c > confidence) {
                confidence = c;
                encoding = prober_encoding;
            }

            if (i.first == KEncodingProber::ChineseTraditional && c < 0.5) {
                // test Big5
                c = codecConfidenceForData(QTextCodec::codecForName("Big5"), data, i.second);

                if (c > 0.5 && c > confidence) {
                    confidence = c;
                    encoding = "Big5";
                }
            }
        }

        if (i.first != proberType) {
            // 使用 proberType 类型探测出的结果结合此国家再次做编码检查
            i.first = proberType;
            prober_confidence = pre_confidence;
            prober_encoding = pre_encoding;
            goto confidence;
        }
    }

    if (def_codec && codecConfidenceForData(def_codec, data, QLocale::system().country()) > confidence) {
        return def_codec->name();
    }

    return encoding;
}

QString Utils::toShortString(QString strSrc, int limitCounts, int left)
{
    left = (left >= limitCounts || left <= 0) ? limitCounts / 2 : left;
    int right = limitCounts - left;
    QString displayName = "";
    displayName = strSrc.length() > limitCounts ? strSrc.left(left) + "..." + strSrc.right(right) : strSrc;
    return displayName;
}

bool Utils::checkAndDeleteDir(const QString &iFilePath)
{
    QFileInfo tempFileInfo(iFilePath);

    if (tempFileInfo.isDir()) {
        deleteDir(iFilePath);
        return true;
    } else if (tempFileInfo.isFile()) {
        QFile deleteFile(iFilePath);
        return  deleteFile.remove();
    }
    return false;
}

bool Utils::deleteDir(const QString &iFilePath)
{
    QDir directory(iFilePath);
    if (!directory.exists()) {
        return true;
    }

    QString srcPath = QDir::toNativeSeparators(iFilePath);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();

    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    bool error = false;
    for (QStringList::size_type i = 0; i != fileNames.size(); ++i) {
        QString filePath = srcPath + fileNames.at(i);
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile() || fileInfo.isSymLink()) {
            QFile::setPermissions(filePath, QFile::WriteOwner);
            if (!QFile::remove(filePath)) {
                qDebug() << "remove file" << filePath << " faild!";
                error = true;
            }
        } else if (fileInfo.isDir()) {
            if (!deleteDir(filePath)) {
                error = true;
            }
        }
    }

    if (!directory.rmdir(QDir::toNativeSeparators(directory.path()))) {
        qDebug() << "remove dir" << directory.path() << " faild!";
        error = true;
    }

    return !error;

}

QString Utils::readConf()
{
    const QString confDir = DStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(confDir + QDir::separator())) {
        dir.mkpath(confDir + QDir::separator());
    }
    const QString confPath = confDir + QDir::separator() + "deepin-compressor.confbf";
    QFile confFile(confPath);

    // default settings
    if (!confFile.exists()) {
        confFile.open(QIODevice::WriteOnly | QIODevice::Text);

        foreach (QString key, m_associtionlist) {
            QString content = key + ":" + "true" + "\n";
            confFile.write(content.toUtf8());
        }

        confFile.close();
    }

    QString confValue;
    bool readStatus = confFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (readStatus) {
        confValue = confFile.readAll();
    }
    confFile.close();

    return confValue;
}

bool Utils::existMimeType(QString mimetype)
{
    QString conf = readConf();
    QStringList confList = conf.split("\n", QString::SkipEmptyParts);

    for (int i = 0; i < confList.count(); i++) {
        qDebug() << confList.at(i);
    }
    bool exist = false;
    for (int i = 0; i < confList.count(); i++) {
        if (confList.at(i).contains("." + mimetype + ":")) {
            if (confList.at(i).contains("true")) {
                exist = true;
                break;
            } else {
                exist = false;
                continue;
            }
        }
    }

    return exist;
}

bool Utils::existArchiveType(QString mimetype, bool &bArchive)
{
    QString conf = readConf();
    QStringList confList = conf.split("\n", QString::SkipEmptyParts);

    for (int i = 0; i < confList.count(); i++) {
        qDebug() << confList.at(i);
    }
    bool exist = false;
    bArchive = false;
    for (int i = 0; i < confList.count(); i++) {
        if (confList.at(i).contains("." + mimetype + ":")) {
            bArchive = true;
            if (confList.at(i).contains("true")) {
                exist = true;
                break;
            } else {
                exist = false;
                continue;
            }
        }
    }

    return exist;
}

QString Utils::judgeFileMime(QString file)
{
    QString type = "";
    if (file.endsWith(".7z") || file.contains(".7z.0")) {
        type = "x-7z-compressed";
    } else if (file.endsWith(".cpio.gz")) {
        type = "x-cpio-compressed";
    } else if (file.endsWith(".tar.bz2")) {
        type = "x-bzip-compressed-tar";
    } else if (file.endsWith(".tar.gz")) {
        type = "x-compressed-tar";
    } else if (file.endsWith(".tar.lz")) {
        type = "x-lzip-compressed-tar";
    } else if (file.endsWith(".tar.lzma")) {
        type = "x-lzma-compressed-tar";
    } else if (file.endsWith(".tar.lzo")) {
        type = "x-tzo";
    } else if (file.endsWith(".tar.xz")) {
        type = "x-xz-compressed-tar";
    } else if (file.endsWith(".tar.Z")) {
        type = "x-tarz";
    } else if (file.endsWith(".src.rpm")) {
        type = "x-source-rpm";
    } else if (file.endsWith(".ar")) {
        type = "x-archive";
    } else if (file.endsWith(".bcpio")) {
        type = "x-bcpio";
    } else if (file.endsWith(".bz2")) {
        type = "x-bzip";
    } else if (file.endsWith(".cpio")) {
        type = "x-cpio";
    } else if (file.endsWith(".deb")) {
        type = "vnd.debian.binary-package";
    } else if (file.endsWith(".gz")) {
        type = "gzip";
    } else if (file.endsWith(".jar")) {
        type = "x-java-archive";
    } else if (file.endsWith(".lzma")) {
        type = "x-lzma";
    } else if (file.endsWith(".cab")) {
        type = "vnd.ms-cab-compressed";
    } else if (file.endsWith(".rar")) {
        type = "vnd.rar";
    } else if (file.endsWith(".rpm")) {
        type = "x-rpm";
    } else if (file.endsWith(".sv4cpio")) {
        type = "x-sv4cpio";
    } else if (file.endsWith(".sc4crc")) {
        type = "x-sc4crc";
    } else if (file.endsWith(".tar")) {
        type = "x-tar";
    } else if (file.endsWith(".xar")) {
        type = "x-xar";
    } else if (file.endsWith(".xz")) {
        type = "x-xz";
    } else if (file.endsWith(".zip")) {
        type = "zip";
    } else if (file.endsWith(".iso")) {
        type = "x-cd-image";
    } else if (file.endsWith(".appimage")) {
        type = "x-iso9660-appimage";
    }

    return type;
}
