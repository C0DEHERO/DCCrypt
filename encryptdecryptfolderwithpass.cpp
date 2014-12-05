#include "encryptdecryptfolderwithpass.h"
#include <QMessageBox>

EncryptDecryptFolderWithPass::EncryptDecryptFolderWithPass(QObject *parent) :
    EncryptDecryptFileWithPass(parent)
{
    stopped = false;
    processSubdirs = true;
    followSymLinks = true;
    settingsFileName = QDir::toNativeSeparators(QString::fromAscii("settings.cfg"));
}

void EncryptDecryptFolderWithPass::run()
{
    stopped = false;
    readSettings(settingsFileName);
    startOperation(inputFolderPath);
}

void EncryptDecryptFolderWithPass::stopQueue()
{
    stopped = true;
}

void EncryptDecryptFolderWithPass::startOperation(const QString &folderPath)
{
    QDir dir(folderPath);
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Size | QDir::DirsFirst);

    if(dir.exists())
    {
        QFileInfoList list = dir.entryInfoList();

        for(int i = 0; i < list.size(); i++)
        {
            if(stopped)
            {
                //emit canceled();  // already emitted by EncryptDecryptFolderWithPass::EncryptDecryptFileWithPass
                break;
            }
            QFileInfo fileInfo = list.at(i);

            if(fileInfo.isDir())
            {
                if(!processSubdirs)
                    break;

                if(fileInfo.exists())
                 {
                    QString outputFolderPathBak(this->outputFolderPath);  //creates a backup of the old output folder path so it can be restored easily

                    QString outputPath;
                    outputPath = outputFolderPath + QString::fromAscii("/") + fileInfo.fileName();

                    if(fileInfo.isSymLink())
                    {
                        if(!followSymLinks)
                            break;

                        outputPath.clear();

                        QFileInfo symLinkTarget(fileInfo.symLinkTarget());

                        outputPath = outputFolderPath + QString::fromAscii("/") + symLinkTarget.fileName();
                    }

                    outputPath = QDir::toNativeSeparators(outputPath);

                    QDir tmpDir;
                    tmpDir.mkpath(outputPath);

                    setOutputFolderPath(outputPath);

                    startOperation(fileInfo.absoluteFilePath());
                    setOutputFolderPath(outputFolderPathBak);
                }
            }
            else if(fileInfo.isFile())
            {
                if(fileInfo.exists())
                {
                    QString outputFileName;
                    outputFileName = outputFolderPath + QString::fromAscii("/") + fileInfo.fileName();

                    if(fileInfo.isSymLink())
                    {
                        if(!followSymLinks)
                            break;

                        outputFileName.clear();

                        QFileInfo symLinkTarget(fileInfo.symLinkTarget());

                        outputFileName = outputFolderPath + QString::fromAscii("/") + symLinkTarget.fileName();
                    }

                    outputFileName = QDir::toNativeSeparators(outputFileName);

                    {
                        QString currentFileName = fileInfo.dir().dirName() + QString::fromAscii("/") + fileInfo.fileName();
                        currentFileName = QDir::toNativeSeparators(currentFileName);

                        emit currentFile(currentFileName);
                    }
                    encryptDecrypt(fileInfo.absoluteFilePath(), outputFileName, password);
                }
            }
        }
        if(folderPath == inputFolderPath)
            if(!stopped)
                emit successfullyFinishedAll();

        stopped = false;
    }
}

void EncryptDecryptFolderWithPass::setInputFolderPath(const QString &inputFolderPath)
{
    this->inputFolderPath = inputFolderPath;
}

void EncryptDecryptFolderWithPass::setOutputFolderPath(const QString &outputFolderPath)
{
    this->outputFolderPath = outputFolderPath;
}

void EncryptDecryptFolderWithPass::setPassword(const QString &password)
{
    this->password = password;
}

void EncryptDecryptFolderWithPass::readSettings(QString settingsFileName)
{
    bool anyErrorOccurred = false;
    QString errorString;

    QFile settingsFile(settingsFileName);
    if(!settingsFile.open(QFile::ReadOnly))
    {
        errorString.append(tr("Could not open \"%1\"!\n").arg(settingsFileName));
        anyErrorOccurred = true;
    }

    QDataStream settingsFileStream(&settingsFile);
    settingsFileStream.setVersion(QDataStream::Qt_4_7);


    quint32 magicNumber;

    settingsFileStream >> magicNumber;

    if(!anyErrorOccurred)
    {
        if(magicNumber != quint32(0x47659436))
        {
            errorString.append(tr("\"%1\" is not a Datacrypt settings file!\n").arg(settingsFileName));
            anyErrorOccurred = true;
        }
    }

    if(anyErrorOccurred)
    {
        emit errorsOccurred(0, tr("Errors Occurred!"), errorString);
    }
    else
    {
        settingsFileStream >> processSubdirs;
        settingsFileStream >> followSymLinks;
    }
}
