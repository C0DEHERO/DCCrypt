#include "folderencryptdecryptdialog.h"
#include "ui_folderencryptdecryptdialog.h"
#include "encryptdecryptfolder.h"
#include "encryptdecryptfolderwithpass.h"
#include <QCloseEvent>
#include <QSettings>

FolderEncryptDecryptDialog::FolderEncryptDecryptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FolderEncryptDecryptDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    setMinimumSize(size());

    ui->repeatPasswordLabel->hide();
    ui->repeatPasswordEdit->hide();
    ui->repeatPasswordEdit->setEchoMode(QLineEdit::Password);

    keyIsFile = true;

    encryptDecryptFolder = new EncryptDecryptFolder;
    encryptDecryptFolderWithPass = new EncryptDecryptFolderWithPass;

    connect(ui->startButton, SIGNAL(pressed()), this, SLOT(pressedStartButton()));
    connect(ui->keyFileRadioButton, SIGNAL(toggled(bool)), this, SLOT(setKeyToFile(bool)));
    connect(ui->passwordRadioButton, SIGNAL(toggled(bool)), this, SLOT(setKeyToPassword(bool)));
    connect(ui->stopSlider, SIGNAL(valueChanged(int)), this, SLOT(stop(int)));

    connect(ui->inputLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkAll()));
    connect(ui->outputLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkAll()));
    connect(ui->keyLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkAll()));
    connect(ui->repeatPasswordEdit, SIGNAL(textEdited(QString)), this, SLOT(checkAll()));

    connect(ui->browseInputButton, SIGNAL(pressed()), this, SLOT(browseInputFolder()));
    connect(ui->browseOutputButton, SIGNAL(pressed()), this, SLOT(browseOutputFolder()));
    connect(ui->browseKeyButton, SIGNAL(pressed()), this, SLOT(browseKeyFile()));

    connect(encryptDecryptFolder, SIGNAL(currentFile(QString)), this, SLOT(updateGroupBoxTitle(QString)));
    connect(encryptDecryptFolder, SIGNAL(fileSize(int)), ui->progressBar, SLOT(setMaximum(int)), Qt::UniqueConnection);
    connect(encryptDecryptFolder, SIGNAL(madeProgress(int)), ui->progressBar, SLOT(setValue(int)), Qt::UniqueConnection);

    connect(encryptDecryptFolder, SIGNAL(successfullyFinishedAll()), this, SLOT(finished()), Qt::UniqueConnection);
    connect(encryptDecryptFolderWithPass, SIGNAL(successfullyFinishedAll()), this, SLOT(finished()), Qt::UniqueConnection);

    connect(this, SIGNAL(finallyStop()), encryptDecryptFolder, SLOT(stop()), Qt::UniqueConnection);
    connect(this, SIGNAL(finallyStop()), encryptDecryptFolder, SLOT(stopQueue()), Qt::UniqueConnection);
    connect(encryptDecryptFolder, SIGNAL(canceled()), this, SLOT(canceled()), Qt::UniqueConnection);

    connect(encryptDecryptFolder, SIGNAL(errorsOccurred(int,QString,QString)), this, SLOT(showErrors(int,QString,QString)), Qt::UniqueConnection);

    checkAll();

    ui->progressGroupBox->hide();

    keyIsFile = true;
}

FolderEncryptDecryptDialog::~FolderEncryptDecryptDialog()
{
    delete ui;
}

void FolderEncryptDecryptDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}



void FolderEncryptDecryptDialog::pressedStartButton()
{
    ui->inputLabel->setEnabled(false);
    ui->inputLineEdit->setEnabled(false);
    ui->browseInputButton->setEnabled(false);

    ui->outputLabel->setEnabled(false);
    ui->outputLineEdit->setEnabled(false);
    ui->browseOutputButton->setEnabled(false);

    ui->keyLabel->setEnabled(false);
    ui->keyLineEdit->setEnabled(false);
    ui->browseKeyButton->setEnabled(false);

    ui->startButton->hide();

    ui->encryptWithLabel->setEnabled(false);
    ui->keyFileRadioButton->setEnabled(false);
    ui->passwordRadioButton->setEnabled(false);

    ui->repeatPasswordLabel->setEnabled(false);
    ui->repeatPasswordEdit->setEnabled(false);

    ui->progressGroupBox->show();

    if(keyIsFile)
    {
        encryptDecryptFolder->setInputFolderPath(ui->inputLineEdit->text());
        encryptDecryptFolder->setOutputFolderPath(ui->outputLineEdit->text());
        encryptDecryptFolder->setKeyFileName(ui->keyLineEdit->text());

        checkAll();

        encryptDecryptFolder->start();
        setWindowModified(true);
    }
    else
    {
        encryptDecryptFolderWithPass->setInputFolderPath(ui->inputLineEdit->text());
        encryptDecryptFolderWithPass->setOutputFolderPath(ui->outputLineEdit->text());
        encryptDecryptFolderWithPass->setPassword(ui->keyLineEdit->text());

        checkAll();

        encryptDecryptFolderWithPass->start();
        setWindowModified(true);
    }
}

void FolderEncryptDecryptDialog::setKeyToFile(bool toggled)
{
    if(toggled)
    {
        keyIsFile = true;
        ui->keyLabel->setText(tr("Key File:"));

        ui->keyLineEdit->setText(QString::fromAscii(""));
        ui->repeatPasswordEdit->setText(QString::fromAscii(""));

        ui->keyLineEdit->setEchoMode(QLineEdit::Normal);
        ui->browseKeyButton->show();

        ui->repeatPasswordLabel->hide();
        ui->repeatPasswordEdit->hide();

        ui->keyLabel->setToolTip(ui->browseKeyButton->toolTip());
        ui->keyLineEdit->setToolTip(ui->browseKeyButton->toolTip());

        checkKeyFile(ui->keyLineEdit->text());

        disconnect(encryptDecryptFolderWithPass, SIGNAL(fileSize(int)), ui->progressBar, SLOT(setMaximum(int)));
        disconnect(encryptDecryptFolderWithPass, SIGNAL(madeProgress(int)), ui->progressBar, SLOT(setValue(int)));
        disconnect(encryptDecryptFolderWithPass, SIGNAL(successfullyFinishedAll()), this, SLOT(finished()));
        disconnect(this, SIGNAL(finallyStop()), encryptDecryptFolderWithPass, SLOT(stop()));
        disconnect(this, SIGNAL(finallyStop()), encryptDecryptFolderWithPass, SLOT(stopQueue()));
        disconnect(encryptDecryptFolderWithPass, SIGNAL(canceled()), this, SLOT(canceled()));
        disconnect(encryptDecryptFolderWithPass, SIGNAL(errorsOccurred(int,QString,QString)), this, SLOT(showErrors(int,QString,QString)));
        disconnect(encryptDecryptFolderWithPass, SIGNAL(currentFile(QString)), this, SLOT(updateGroupBoxTitle(QString)));

        connect(encryptDecryptFolder, SIGNAL(fileSize(int)), ui->progressBar, SLOT(setMaximum(int)), Qt::UniqueConnection);
        connect(encryptDecryptFolder, SIGNAL(madeProgress(int)), ui->progressBar, SLOT(setValue(int)), Qt::UniqueConnection);
        connect(encryptDecryptFolder, SIGNAL(successfullyFinishedAll()), this, SLOT(finished()), Qt::UniqueConnection);
        connect(this, SIGNAL(finallyStop()), encryptDecryptFolder, SLOT(stop()), Qt::UniqueConnection);
        connect(this, SIGNAL(finallyStop()), encryptDecryptFolder, SLOT(stopQueue()), Qt::UniqueConnection);
        connect(encryptDecryptFolder, SIGNAL(canceled()), this, SLOT(canceled()), Qt::UniqueConnection);
        connect(encryptDecryptFolder, SIGNAL(errorsOccurred(int,QString,QString)), this, SLOT(showErrors(int,QString,QString)), Qt::UniqueConnection);
        connect(encryptDecryptFolder, SIGNAL(currentFile(QString)), this, SLOT(updateGroupBoxTitle(QString)), Qt::UniqueConnection);
    }
}

void FolderEncryptDecryptDialog::setKeyToPassword(bool toggled)
{
    if(toggled)
    {
        keyIsFile = false;
        ui->keyLabel->setText(tr("Password:"));

        ui->keyLineEdit->setText(QString::fromAscii(""));
        ui->repeatPasswordEdit->setText(QString::fromAscii(""));

        ui->keyLineEdit->setEchoMode(QLineEdit::Password);
        ui->browseKeyButton->hide();
        ui->repeatPasswordLabel->show();
        ui->repeatPasswordEdit->show();

        ui->keyLabel->setToolTip(ui->repeatPasswordLabel->toolTip());
        ui->keyLineEdit->setToolTip(ui->repeatPasswordLabel->toolTip());

        checkPassword(ui->keyLineEdit->text());
        checkRepeatedPassword(ui->repeatPasswordEdit->text());

        disconnect(encryptDecryptFolder, SIGNAL(fileSize(int)), ui->progressBar, SLOT(setMaximum(int)));
        disconnect(encryptDecryptFolder, SIGNAL(madeProgress(int)), ui->progressBar, SLOT(setValue(int)));
        disconnect(encryptDecryptFolder, SIGNAL(successfullyFinishedAll()), this, SLOT(finished()));
        disconnect(this, SIGNAL(finallyStop()), encryptDecryptFolder, SLOT(stop()));
        disconnect(this, SIGNAL(finallyStop()), encryptDecryptFolder, SLOT(stopQueue()));
        disconnect(encryptDecryptFolder, SIGNAL(canceled()), this, SLOT(canceled()));
        disconnect(encryptDecryptFolder, SIGNAL(errorsOccurred(int,QString,QString)), this, SLOT(showErrors(int,QString,QString)));
        disconnect(encryptDecryptFolder, SIGNAL(currentFile(QString)), this, SLOT(updateGroupBoxTitle(QString)));

        connect(encryptDecryptFolderWithPass, SIGNAL(fileSize(int)), ui->progressBar, SLOT(setMaximum(int)), Qt::UniqueConnection);
        connect(encryptDecryptFolderWithPass, SIGNAL(madeProgress(int)), ui->progressBar, SLOT(setValue(int)), Qt::UniqueConnection);
        connect(encryptDecryptFolderWithPass, SIGNAL(successfullyFinishedAll()), this, SLOT(finished()), Qt::UniqueConnection);
        connect(this, SIGNAL(finallyStop()), encryptDecryptFolderWithPass, SLOT(stop()), Qt::UniqueConnection);
        connect(this, SIGNAL(finallyStop()), encryptDecryptFolderWithPass, SLOT(stopQueue()), Qt::UniqueConnection);
        connect(encryptDecryptFolderWithPass, SIGNAL(canceled()), this, SLOT(canceled()), Qt::UniqueConnection);
        connect(encryptDecryptFolderWithPass, SIGNAL(errorsOccurred(int,QString,QString)), this, SLOT(showErrors(int,QString,QString)), Qt::UniqueConnection);
        connect(encryptDecryptFolderWithPass, SIGNAL(currentFile(QString)), this, SLOT(updateGroupBoxTitle(QString)), Qt::UniqueConnection);
    }
}

bool FolderEncryptDecryptDialog::stop(int value)
{
    if(value >= 100)
    {
        int i = QMessageBox::warning(this,tr("Encrypt Folder"), tr("Do you really want to stop the operation?"),
                                     QMessageBox::Yes, QMessageBox::No | QMessageBox::Escape);

        if(i == QMessageBox::Yes)
        {
            restoreUi();
            emit finallyStop();
            return true;
        }
        else if(i == QMessageBox::No)
        {
            ui->stopSlider->setValue(0);
            return false;
        }
    }

    return false;
}

void FolderEncryptDecryptDialog::finished()
{
#ifdef Q_OS_LINUX
    if(ui->shutdownCheckBox->isChecked())
        system("shutdown -h 0");
#endif

#ifdef Q_OS_WIN32
    if(ui->shutdownCheckBox->isChecked())
        system("shutdown -s -t 00");
#endif

#ifdef Q_OS_WIN64
    if(ui->shutdownCheckBox->isChecked())
        system("shutdown -s -t 00");
#endif

    if(!ui->shutdownCheckBox->isChecked())
    QMessageBox::information(this, tr("Finished"), tr("Successfully finished all operations."), QMessageBox::Ok);
    restoreUi();
}

void FolderEncryptDecryptDialog::restoreUi()
{
    setWindowModified(false);

    ui->inputLabel->setEnabled(true);
    ui->inputLineEdit->setEnabled(true);
    ui->browseInputButton->setEnabled(true);

    ui->outputLabel->setEnabled(true);
    ui->outputLineEdit->setEnabled(true);
    ui->browseOutputButton->setEnabled(true);

    ui->keyLabel->setEnabled(true);
    ui->keyLineEdit->setEnabled(true);
    ui->browseKeyButton->setEnabled(true);

    ui->repeatPasswordLabel->setEnabled(true);
    ui->repeatPasswordEdit->setEnabled(true);

    ui->startButton->show();

    ui->encryptWithLabel->setEnabled(true);
    ui->keyFileRadioButton->setEnabled(true);
    ui->passwordRadioButton->setEnabled(true);

    ui->progressGroupBox->hide();
    ui->stopSlider->setValue(0);
    ui->progressBar->setValue(0);
    ui->shutdownCheckBox->setChecked(false);

    ui->progressGroupBox->hide();
}

void FolderEncryptDecryptDialog::showErrors(int errorCode, QString title, QString text)
{
    if(errorCode == 0)
    {
        QMessageBox::critical(this, title, text,
                              QMessageBox::Ok | QMessageBox::Default);
    }
    else if(errorCode > 0)
    {
        QMessageBox::critical(this, title, tr("Error: %1").arg(QString::number(errorCode)).append(text),
                              QMessageBox::Ok | QMessageBox::Default);
    }
    else if(errorCode < 0)
    {

    }
}

void FolderEncryptDecryptDialog::checkInputFolder(QString inputFolderName)
{
    if(inputFolderName.isEmpty())
    {
        inputFolderStatus = FolderEncryptDecryptDialog::NonChosen;
        applyCheckToUi();
        return;
    }

    if(inputFolderName == ui->outputLineEdit->text())
    {
        inputFolderStatus = FolderEncryptDecryptDialog::Equal;
        applyCheckToUi();
        return;
    }

    QDir inputFolder(inputFolderName);
    if(!inputFolder.exists())
    {
        inputFolderStatus = FolderEncryptDecryptDialog::DoesntExist;
    }
    else
    {
        inputFolderStatus = FolderEncryptDecryptDialog::Exists;
    }
    applyCheckToUi();
}

void FolderEncryptDecryptDialog::checkOutputFolder(QString outputFolderPath)
{
    if(outputFolderPath.isEmpty())
    {
        outputFolderStatus = FolderEncryptDecryptDialog::NonChosen;
        applyCheckToUi();
        return;
    }

    if(outputFolderPath == ui->inputLineEdit->text())
    {
        outputFolderStatus = FolderEncryptDecryptDialog::Equal;
        applyCheckToUi();
        return;
    }

    QDir outputFolder(outputFolderPath);
    if(!outputFolder.exists())
    {
        outputFolderStatus = FolderEncryptDecryptDialog::DoesntExist;
    }
    else
    {
        outputFolderStatus = FolderEncryptDecryptDialog::Exists;
    }
    applyCheckToUi();
}

void FolderEncryptDecryptDialog::checkKeyFile(QString keyFileName)
{
    if(keyFileName.isEmpty())
    {
        keyFileStatus = FolderEncryptDecryptDialog::NonChosen;
        applyCheckToUi();
        return;
    }

    QFile keyFile(keyFileName);
    if(!keyFile.exists())
    {
        keyFileStatus = FolderEncryptDecryptDialog::DoesntExist;
    }
    else
    {
        keyFileStatus = FolderEncryptDecryptDialog::Exists;
    }
    applyCheckToUi();
}

void FolderEncryptDecryptDialog::checkPassword(QString password)
{
    if(password.isEmpty() && ui->repeatPasswordEdit->text().isEmpty())
    {
        passwordStatus = FolderEncryptDecryptDialog::NonGiven;
        applyCheckToUi();
        return;
    }
    if(password == ui->repeatPasswordEdit->text())
        passwordStatus = FolderEncryptDecryptDialog::Equal;
    else
        passwordStatus = FolderEncryptDecryptDialog::NotEqual;

    applyCheckToUi();
}

void FolderEncryptDecryptDialog::checkRepeatedPassword(QString repeatedPassword)
{
    if(repeatedPassword.isEmpty() && ui->keyLineEdit->text().isEmpty())
    {
        passwordStatus = FolderEncryptDecryptDialog::NonGiven;
        applyCheckToUi();
        return;
    }
    if(repeatedPassword == ui->keyLineEdit->text())
        passwordStatus = FolderEncryptDecryptDialog::Equal;
    else
        passwordStatus = FolderEncryptDecryptDialog::NotEqual;

    applyCheckToUi();
}

void FolderEncryptDecryptDialog::checkAll()
{
    checkInputFolder(ui->inputLineEdit->text());
    checkOutputFolder(ui->outputLineEdit->text());
    if(keyIsFile)
        checkKeyFile(ui->keyLineEdit->text());
    else
    {
        checkPassword(ui->keyLineEdit->text());
        checkRepeatedPassword(ui->repeatPasswordEdit->text());
    }
}

void FolderEncryptDecryptDialog::applyCheckToUi()
{
    if(inputFolderStatus == FolderEncryptDecryptDialog::NonChosen)
    {
        ui->inputFolderStatusLabel->setText(tr("No folder chosen!"));
        ui->inputFolderStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
    }
    else if(inputFolderStatus == FolderEncryptDecryptDialog::DoesntExist)
    {
        ui->inputFolderStatusLabel->setText(tr("Doesn't exist!"));
        ui->inputFolderStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
    }
    else if(inputFolderStatus == FolderEncryptDecryptDialog::Exists)
    {
        ui->inputFolderStatusLabel->setText(tr("Exists"));
        ui->inputFolderStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(0, 170, 0);"));
    }
    else if(inputFolderStatus == FolderEncryptDecryptDialog::Equal)
    {
        ui->inputFolderStatusLabel->setText(tr("Already output folder!"));
        ui->inputFolderStatusLabel->setStyleSheet(QString::fromAscii("color:rgb(170, 0, 0)"));
    }

    if(outputFolderStatus == FolderEncryptDecryptDialog::NonChosen)
    {
        ui->outputFolderStatusLabel->setText(tr("No folder chosen!"));
        ui->outputFolderStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
    }
    else if(outputFolderStatus == FolderEncryptDecryptDialog::DoesntExist)
    {
        ui->outputFolderStatusLabel->setText(tr("Doesn't exist!"));
        ui->outputFolderStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
    }
    else if(outputFolderStatus == FolderEncryptDecryptDialog::Exists)
    {
        ui->outputFolderStatusLabel->setText(tr("Exists"));
        ui->outputFolderStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(0, 170, 0);"));
    }
    else if(outputFolderStatus == FolderEncryptDecryptDialog::Equal)
    {
        ui->outputFolderStatusLabel->setText(tr("Already input folder!"));
        ui->outputFolderStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
    }

    if(keyIsFile)
    {
        if(keyFileStatus == FolderEncryptDecryptDialog::NonChosen)
        {
            ui->keyFileStatusLabel->setText(tr("No file chosen!"));
            ui->keyFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
        }
        else if(keyFileStatus == FolderEncryptDecryptDialog::DoesntExist)
        {
            ui->keyFileStatusLabel->setText(tr("Doesn't exist."));
            ui->keyFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
        }
        else if(keyFileStatus == FolderEncryptDecryptDialog::Exists)
        {
            ui->keyFileStatusLabel->setText(tr("Exists"));
            ui->keyFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(0, 170, 0);"));
        }

        if(inputFolderStatus != FolderEncryptDecryptDialog::Exists || outputFolderStatus != FolderEncryptDecryptDialog::Exists || keyFileStatus != FolderEncryptDecryptDialog::Exists)
        {
            ui->startButton->setEnabled(false);
        }
        else if(inputFolderStatus == FolderEncryptDecryptDialog::Exists && outputFolderStatus == FolderEncryptDecryptDialog::Exists && keyFileStatus == FolderEncryptDecryptDialog::Exists)
        {
            ui->startButton->setEnabled(true);
        }
    }
    else if(!keyIsFile)
    {
        if(passwordStatus == FolderEncryptDecryptDialog::NonGiven)
        {
            ui->keyFileStatusLabel->setText(tr("Non given!"));
            ui->keyFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
        }
        else if(passwordStatus == FolderEncryptDecryptDialog::NotEqual)
        {
            ui->keyFileStatusLabel->setText(tr("Not equal!"));
            ui->keyFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
        }
        else if(passwordStatus == FolderEncryptDecryptDialog::Equal)
        {
            ui->keyFileStatusLabel->setText(tr("Equal"));
            ui->keyFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(0, 170, 0);"));
        }

        if(inputFolderStatus != FolderEncryptDecryptDialog::Exists || outputFolderStatus != FolderEncryptDecryptDialog::Exists || passwordStatus != FolderEncryptDecryptDialog::Equal)
        {
            ui->startButton->setEnabled(false);
        }
        else if(inputFolderStatus == FolderEncryptDecryptDialog::Exists && outputFolderStatus == FolderEncryptDecryptDialog::Exists && passwordStatus == FolderEncryptDecryptDialog::Equal)
        {
            ui->startButton->setEnabled(true);
        }
    }
}

void FolderEncryptDecryptDialog::browseInputFolder()
{

    QString home(QDir::homePath());
    home = QDir::toNativeSeparators(home);

    ui->inputLineEdit->setText(QFileDialog::getExistingDirectory(this,
                                                            tr("Open Input Directory"),
                                                            home,
                                                            QFileDialog::ShowDirsOnly));
}

void FolderEncryptDecryptDialog::browseOutputFolder()
{
    QString home(QDir::homePath());
    home = QDir::toNativeSeparators(home);

    ui->outputLineEdit->setText(QFileDialog::getExistingDirectory(this,
                                                            tr("Open Output Directory"),
                                                            home,
                                                            QFileDialog::ShowDirsOnly));
}

void FolderEncryptDecryptDialog::browseKeyFile()
{
    QString home(QDir::homePath());
    home = QDir::toNativeSeparators(home);

    ui->keyLineEdit->setText(QFileDialog::getOpenFileName(this,
                                                          tr("Open Key File"),
                                                          home));
}

void FolderEncryptDecryptDialog::canceled()
{
    QMessageBox::information(this, tr("Canceled"), tr("Canceled by user."), QMessageBox::Ok);
    restoreUi();
}

void FolderEncryptDecryptDialog::updateGroupBoxTitle(QString currentFile)
{
    ui->progressGroupBox->setTitle(tr("Encrypting/Decrypting: ") + currentFile);
}

void FolderEncryptDecryptDialog::closeEvent(QCloseEvent *event)
{
    if(isWindowModified())
    {
        if(stop(100))
            event->accept();
        else
            event->ignore();
    }
}
