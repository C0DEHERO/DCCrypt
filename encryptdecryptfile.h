#ifndef ENCRYPTDECRYPTFILE_H
#define ENCRYPTDECRYPTFILE_H

#include <QThread>
#include <QFile>

class EncryptDecryptFile : public QThread
{
    Q_OBJECT
public:
    explicit EncryptDecryptFile(QObject *parent = 0);
    void run();
    void encryptDecrypt(const QString &inputFileName, const QString &outputFileName, const QString &keyFileName);


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
    void setKeyFileName(const QString&);
    void setUpdateInterval(const qint64&);

private:
    inline void encryptDecryptChar(char &inputOutputChar, const char &keyChar);
    inline void encryptDecryptChar(const char &inputChar, const char &keyChar, char &outputChar);
    volatile bool stopped;
    QString inputFileName, outputFileName, keyFileName;
    qint64 updateInterval;

};

#endif // ENCRYPTDECRYPTFILE_H
