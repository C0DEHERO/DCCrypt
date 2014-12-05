#include "encryptdecryptfile.h"
#include <cstdio>

EncryptDecryptFile::EncryptDecryptFile(QObject *parent) :
    QThread(parent)
{
    stopped = false;
    updateInterval = 1048576;  //set the update interval of the progress bar to 1MB to update every time a MB was processed
}

void EncryptDecryptFile::run()
{
    stopped = false;
    encryptDecrypt(inputFileName, outputFileName, keyFileName);
}

void EncryptDecryptFile::stop()
{
    stopped = true;
}

bool EncryptDecryptFile::isStopped()
{
    return stopped;
}

void EncryptDecryptFile::encryptDecrypt(const QString &inputFileName, const QString &outputFileName, const QString &keyFileName)
{
    QFile inputFile(inputFileName);
    QFile outputFile(outputFileName);
    QFile keyFile(keyFileName);

        QString errorString;
        bool anErrorOccurred = false;
        register int i = 0;

        if(!inputFile.open(QIODevice::ReadOnly))
        {
            errorString.append(tr("Could not open \"%1\" for reading.\n").arg(inputFileName));
            anErrorOccurred = true;
        }
        else
            errorString.append(tr("Could successfully open \"%1\" for reading.\n").arg(inputFileName));

        if(!outputFile.open(QIODevice::WriteOnly))
        {
            errorString.append(tr("Could not open \"%1\" for writing.\n").arg(outputFileName));
            anErrorOccurred = true;
        }
        else
            errorString.append(tr("Could successfully open \"%1\" for writing.\n").arg(outputFileName));

        if(!keyFile.open((QIODevice::ReadOnly)))
        {
            errorString.append(tr("Could not open \"%1\" for reading.\n").arg(keyFileName));
            anErrorOccurred = true;
        }
        else
            errorString.append(tr("Could successfully open \"%1\" for reading.\n").arg(keyFileName));

        if(anErrorOccurred)
        {
            emit errorsOccurred(0, tr("Could not open someFiles!"), errorString);
            return;
        }

        char ioChar;
        char keyChar;

        emit fileSize(inputFile.size());

        emit madeProgress(0);

        while(!inputFile.atEnd())
        {
            if(!stopped)
            {
                inputFile.getChar(&ioChar);

                keyFile.getChar(&keyChar);

                encryptDecryptChar(ioChar, keyChar);

                outputFile.putChar(ioChar);

                if(keyFile.atEnd())
                    keyFile.reset();

                if(i >= updateInterval)
                {
                    emit madeProgress(inputFile.pos());
                    i = 0;
                }
                i++;
            }
            else
            {
                emit canceled();
                break;
            }
        }
        inputFile.close();
        outputFile.close();
        keyFile.close();

        if(!stopped)
        {
            emit madeProgress(inputFile.size());
            emit successfullyFinished();
        }
        stopped = false;
}

inline void EncryptDecryptFile::encryptDecryptChar(char &inputOutputChar, const char &keyChar)
{
    inputOutputChar = inputOutputChar^(~keyChar);
}

inline void EncryptDecryptFile::encryptDecryptChar(const char &inputChar, const char &keyChar, char &outputChar)
{
    outputChar = inputChar^(~keyChar);
}

void EncryptDecryptFile::setInputFileName(const QString &inputFileName)
{
    this->inputFileName = inputFileName;
}

void EncryptDecryptFile::setOutputFileName(const QString &outputFileName)
{
    this->outputFileName = outputFileName;
}

void EncryptDecryptFile::setKeyFileName(const QString &keyFileName)
{
    this->keyFileName = keyFileName;
}

void EncryptDecryptFile::setUpdateInterval(const qint64 &updateInterval)
{
    this->updateInterval = updateInterval;
}
