#ifndef CLI7ZPLUGIN_H
#define CLI7ZPLUGIN_H

#include "cliinterface.h"
#include "kpluginfactory.h"

class Cli7zPluginFactory : public KPluginFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.KPluginFactory" FILE "kerfuffle_cli7z.json")
    Q_INTERFACES(KPluginFactory)

public:
    explicit Cli7zPluginFactory();
    ~Cli7zPluginFactory();
};

class Cli7zPlugin : public CliInterface
{
    Q_OBJECT

public:
    explicit Cli7zPlugin(QObject *parent, const QVariantList &args);
    ~Cli7zPlugin() override;

    bool isPasswordPrompt(const QString &line) override;
    bool isWrongPasswordMsg(const QString &line) override;
    bool isCorruptArchiveMsg(const QString &line) override;
    bool isDiskFullMsg(const QString &line) override;
    bool isFileExistsMsg(const QString &line) override;
    bool isFileExistsFileName(const QString &line) override;

private:
    enum ArchiveType {
        ArchiveType7z = 0,
        ArchiveTypeBZip2,
        ArchiveTypeGZip,
        ArchiveTypeXz,
        ArchiveTypeTar,
        ArchiveTypeZip,
        ArchiveTypeRar,
        ArchiveTypeUdf,
        ArchiveTypeIso
    } m_archiveType;

    void setupCliProperties();

    /**
     * @brief readListLine  解析list的命令行输出
     * @param line
     * @return
     */
    bool readListLine(const QString &line);

    bool handleLine(const QString &line, WorkType workStatus) override;

private:
    ParseState m_parseState = ParseStateTitle;
    FileEntry m_fileEntry;
};

#endif // CLI7ZPLUGIN_H