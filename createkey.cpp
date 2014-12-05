#include "createkey.h"
#include <QFile>
#include <QBuffer>

CreateKey::CreateKey(QObject *parent) :
    QThread(parent)
{
    updateInterval = 1048576;
    qsrand(5);
    stopped = false;
}

void CreateKey::run()
{
    stopped = false;
    generateKey(outputKeyFileName, finalKeySize);
}

void CreateKey::stop()
{
    stopped = true;
}

#ifndef Q_OS_LINUX
void CreateKey::generateKey(const QString &outputKeyFile, const qint64 &fileSize)
{
    QFile newKeyFile(outputKeyFile);

    register int i = 0;

    QString errorString;
    bool anErrorOccurred = false;

    if(!newKeyFile.open(QIODevice::WriteOnly))
    {
        errorString.append(tr("Could not open \"%1\" for writing.\n").arg(outputKeyFileName));
        anErrorOccurred = true;
    }
    else
        errorString.append(tr("Could successfully open \"%1\" for writing.\n").arg(outputKeyFileName));

    if(anErrorOccurred)
    {
        emit errorsOccurred(0, tr("Could not open someFiles!"), errorString);
        return;
    }

    while(newKeyFile.pos() < fileSize)
    {
        if(stopped)
        {
            emit canceled();
            break;
        }
        newKeyFile.putChar(qrand());

        if(i >= updateInterval)
        {
            emit madeProgress(newKeyFile.pos());
            i = 0;
        }
        i++;
    }
    emit madeProgress(finalKeySize);

    if(!stopped)
        emit successfullyFinished();
}
#endif

#ifdef Q_OS_LINUX
void CreateKey::generateKey(const QString &outputKeyFileName, const qint64 &fileSize)
{
    QFile newKeyFile(outputKeyFileName);

    QFile urandomDevice(QString::fromAscii("/dev/urandom"));


    QByteArray randomBytes;

    QBuffer randomBuffer(&randomBytes);

    QString errorString;
    bool anErrorOccurred = false;

    if(!newKeyFile.open(QIODevice::WriteOnly))
    {
        errorString.append(tr("Could not open \"%1\" for writing.\n").arg(outputKeyFileName));
        anErrorOccurred = true;
    }
    else
        errorString.append(tr("Could successfully open \"%1\" for writing.\n").arg(outputKeyFileName));

    if(!urandomDevice.open(QIODevice::ReadOnly))
    {
        errorString.append(tr("Could not open \"%1\" for reading.\n").arg(QString::fromAscii("/dev/urandom")));
        anErrorOccurred = true;
    }
    else
        errorString.append(tr("Could successfully open \"%1\" for reading.\n").arg(QString::fromAscii("/dev/urandom")));

    if(!randomBuffer.open(QIODevice::ReadOnly))
    {
        errorString.append(tr("Could not open \"%1\" for reading.\n").arg(tr("the buffer for saving the random numbers")));
        anErrorOccurred = true;
    }
    else
        errorString.append(tr("Could successfully open \"%1\" for reading.\n").arg(tr("the buffer for saving the random numbers")));

    if(anErrorOccurred)
    {
        emit errorsOccurred(0, tr("Could not open someFiles!"), errorString);
        return;
    }

    emit gettingRandomNumbers();
    randomBytes = urandomDevice.read(fileSize);

    char c;

    int i = 0;

    emit creatingKey();

    while(!randomBuffer.atEnd())
    {
        if(stopped)
        {
            emit canceled();
            break;
        }

        randomBuffer.getChar(&c);
        newKeyFile.putChar(c);
        if(i >= updateInterval)
        {
            emit madeProgress(randomBuffer.pos());
            i = 0;
        }
        i++;  
    }

    if(!stopped)
    {
        madeProgress(finalKeySize);
        emit successfullyFinished();
    }

    stopped = false;
}
#endif


void CreateKey::setOutputKeyFileName(const QString &outputKeyFileName)
{
    this->outputKeyFileName = outputKeyFileName;
}

void CreateKey::setFinalKeySize(const qint64 &finalKeySize)
{
    this->finalKeySize = finalKeySize;
}
