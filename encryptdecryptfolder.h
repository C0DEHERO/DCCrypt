#ifndef ENCRYPTDECRYPTFOLDER_H
#define ENCRYPTDECRYPTFOLDER_H

#include "encryptdecryptfile.h"
#include <QDir>

class EncryptDecryptFolder : public EncryptDecryptFile
{
    Q_OBJECT
public:
    explicit EncryptDecryptFolder(QObject *parent = 0);
    void run();

signals:
    void currentFile(const QString&);
    void successfullyFinishedAll();
    void errorsOccurred(const int &, const QString &, const QString &);

public slots:
    void stopQueue();
    void setInputFolderPath(const QString &inputFolderPath);
    void setOutputFolderPath(const QString &outputFolderPath);
    void setKeyFileName(const QString &keyFileName);

private:
    void startOperation(const QString &folderPath);
    QString inputFolderPath, outputFolderPath, keyFileName;
    QString settingsFileName;
    volatile bool stopped;
    bool processSubdirs;
    bool followSymLinks;

private slots:
    void readSettings(QString settingsFileName);
};

#endif // ENCRYPTDECRYPTFOLDER_H
