#ifndef CREATEKEY_H
#define CREATEKEY_H

#include <QThread>

class QFile;

class CreateKey : public QThread
{
    Q_OBJECT
public:
    explicit CreateKey(QObject *parent = 0);
    void run();

    void setOutputKeyFileName(const QString &outputKeyFileName);
    void setFinalKeySize(const qint64 &finalKeySize);

signals:
    void fileSize(int);
    void madeProgress(const int&);
    void errorsOccurred(const int&, const QString&, const QString&);
    void successfullyFinished();
    void canceled();
    void gettingRandomNumbers();
    void creatingKey();

public slots:
    void stop();

private:
    void generateKey(const QString &outputKeyFileName, const qint64 &fileSize);
    void generateKey(const QString &outputKeyFileName, const QString &inputFile);

    QString outputKeyFileName;
    qint64 finalKeySize;
    qint64 updateInterval;

    volatile bool stopped;
};

#endif // CREATEKEY_H
