#ifndef ENCRYPTDECRYPTFILEWITHPASS_H
#define ENCRYPTDECRYPTFILEWITHPASS_H

#include <QThread>
#include <QFile>
#include <QBuffer>

class EncryptDecryptFileWithPass : public QThread
{
    Q_OBJECT
public:
    explicit EncryptDecryptFileWithPass(QObject *parent = 0);
    void run();
    void encryptDecrypt(const QString &inputFileName, const QString &outputFileName, const QString &password);


signals:
    void fileSize(const int&);
    void madeProgress(const int&);
    void errorsOccurred(const int&, const QString&, const QString&);
    void successfullyFinished();
    void canceled();

public slots:
    void stop();
    bool isStopped();
    void setInputFileName(const QString&);
    void setOutputFileName(const QString&);
    void setPassword(const QString&);
    void setUpdateInterval(const qint64&);

private:
    inline void encryptDecryptChar(char &inputOutputChar, const char &keyChar);
    inline void encryptDecryptChar(const char &inputChar, const char &keyChar, char &outputChar);
    volatile bool stopped;
    QString inputFileName, outputFileName, password;
    qint64 updateInterval;

};

#endif // ENCRYPTDECRYPTFILEWITHPASS_H
