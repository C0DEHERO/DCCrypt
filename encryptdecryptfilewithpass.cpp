#include "encryptdecryptfilewithpass.h"

EncryptDecryptFileWithPass::EncryptDecryptFileWithPass(QObject *parent) :
    QThread(parent)
{
    stopped = false;
    updateInterval = 1048576;  //set the update interval of the progress bar to 1MB to update every time a MB was processed
}

void EncryptDecryptFileWithPass::run()
{
    stopped = false;
    encryptDecrypt(inputFileName, outputFileName, password);
}

void EncryptDecryptFileWithPass::stop()
{
    stopped = true;
}

bool EncryptDecryptFileWithPass::isStopped()
{
    return stopped;
}

void EncryptDecryptFileWithPass::encryptDecrypt(const QString &inputFileName, const QString &outputFileName, const QString &password)
{
    QFile inputFile(inputFileName);
    QFile outputFile(outputFileName);
    QByteArray passwordArray = password.toAscii();
    QBuffer passwordBuffer(&passwordArray);

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

        if(!passwordBuffer.open(QIODevice::ReadOnly))
        {
            errorString.append(tr("Could not buffer the password.\n"));
            anErrorOccurred = true;
        }
        else
            errorString.append(tr("Could successfully buffer the password.\n"));

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

                passwordBuffer.getChar(&keyChar);

                encryptDecryptChar(ioChar, keyChar);

                outputFile.putChar(ioChar);

                if(passwordBuffer.atEnd())
                    passwordBuffer.seek(0);

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
        passwordBuffer.close();

        if(!stopped)
            emit madeProgress(inputFile.size());

        if(!stopped)
            emit successfullyFinished();
        stopped = false;
        return;
}

inline void EncryptDecryptFileWithPass::encryptDecryptChar(char &inputOutputChar, const char &keyChar)
{
    inputOutputChar = inputOutputChar^(~keyChar);
}

inline void EncryptDecryptFileWithPass::encryptDecryptChar(const char &inputChar, const char &keyChar, char &outputChar)
{
    outputChar = inputChar^(~keyChar);
}

void EncryptDecryptFileWithPass::setInputFileName(const QString &inputFileName)
{
    this->inputFileName = inputFileName;
}

void EncryptDecryptFileWithPass::setOutputFileName(const QString &outputFileName)
{
    this->outputFileName = outputFileName;
}

void EncryptDecryptFileWithPass::setPassword(const QString &password)
{
    this->password = password;
}

void EncryptDecryptFileWithPass::setUpdateInterval(const qint64 &updateInterval)
{
    this->updateInterval = updateInterval;
}
