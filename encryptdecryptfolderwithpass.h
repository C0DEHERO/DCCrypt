#ifndef ENCRYPTDECRYPTFOLDERWITHPASS_H
#define ENCRYPTDECRYPTFOLDERWITHPASS_H

#include "encryptdecryptfilewithpass.h"
#include <QDir>

class EncryptDecryptFolderWithPass : public EncryptDecryptFileWithPass
{
    Q_OBJECT
public:
    explicit EncryptDecryptFolderWithPass(QObject *parent = 0);
    void run();

signals:
    void currentFile(const QString&);
    void successfullyFinishedAll();
    void errorsOccurred(const int &, const QString &, const QString &);

public slots:
    void stopQueue();
    void setInputFolderPath(const QString &inputFolderPath);
    void setOutputFolderPath(const QString &inputFolderPath);
    void setPassword(const QString &password);

private:
    void startOperation(const QString &folderPath);
    QString inputFolderPath, outputFolderPath, password;
    QString settingsFileName;
    volatile bool stopped;
    bool processSubdirs;
    bool followSymLinks;

private slots:
    void readSettings(QString settingsFileName);

};

#endif // ENCRYPTDECRYPTFOLDERWITHPASS_H
