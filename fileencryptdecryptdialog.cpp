#include "fileencryptdecryptdialog.h"
#include "ui_fileencryptdecryptdialog.h"
#include "encryptdecryptfile.h"
#include "encryptdecryptfilewithpass.h"
#include <QCloseEvent>
#include <QSettings>

FileEncryptDecryptDialog::FileEncryptDecryptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileEncryptDecryptDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    setMinimumSize(size());

    ui->repeatPasswordLabel->hide();
    ui->repeatPasswordEdit->hide();
    ui->repeatPasswordEdit->setEchoMode(QLineEdit::Password);

    keyIsFile = true;

    encryptDecryptFile = new EncryptDecryptFile;
    encryptDecryptFileWithPass = new EncryptDecryptFileWithPass;

    connect(ui->startButton, SIGNAL(pressed()), this, SLOT(pressedStartButton()));
    connect(ui->keyFileRadioButton, SIGNAL(toggled(bool)), this, SLOT(setKeyToFile(bool)));
    connect(ui->passwordRadioButton, SIGNAL(toggled(bool)), this, SLOT(setKeyToPassword(bool)));
    connect(ui->stopSlider, SIGNAL(valueChanged(int)), this, SLOT(stop(int)));

    connect(ui->inputLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkAll()));
    connect(ui->outputLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkAll()));
    connect(ui->keyLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkAll()));
    connect(ui->repeatPasswordEdit, SIGNAL(textEdited(QString)), this, SLOT(checkAll()));

    connect(ui->browseInputButton, SIGNAL(pressed()), this, SLOT(browseInputFile()));
    connect(ui->browseOutputButton, SIGNAL(pressed()), this, SLOT(browseOutputFile()));
    connect(ui->browseKeyButton, SIGNAL(pressed()), this, SLOT(browseKeyFile()));

    connect(encryptDecryptFile, SIGNAL(fileSize(int)), ui->progressBar, SLOT(setMaximum(int)), Qt::UniqueConnection);
    connect(encryptDecryptFile, SIGNAL(madeProgress(int)), ui->progressBar, SLOT(setValue(int)), Qt::UniqueConnection);
    connect(ui->progressBar, SIGNAL(valueChanged(int)), this, SLOT(updateProgressTitle()), Qt::UniqueConnection);

    connect(encryptDecryptFile, SIGNAL(successfullyFinished()), this, SLOT(finished()), Qt::UniqueConnection);

    connect(this, SIGNAL(finallyStop()), encryptDecryptFile, SLOT(stop()), Qt::UniqueConnection);
    connect(encryptDecryptFile, SIGNAL(canceled()), this, SLOT(canceled()), Qt::UniqueConnection);

    connect(encryptDecryptFile, SIGNAL(errorsOccurred(int,QString,QString)), this, SLOT(showErrors(int,QString,QString)), Qt::UniqueConnection);

    checkAll();

    ui->progressGroupBox->hide();

    keyIsFile = true;
}

FileEncryptDecryptDialog::~FileEncryptDecryptDialog()
{
    delete ui;
}

void FileEncryptDecryptDialog::changeEvent(QEvent *e)
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



void FileEncryptDecryptDialog::pressedStartButton()
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
        encryptDecryptFile->setInputFileName(ui->inputLineEdit->text());
        encryptDecryptFile->setOutputFileName(ui->outputLineEdit->text());
        encryptDecryptFile->setKeyFileName(ui->keyLineEdit->text());

        checkAll();

        encryptDecryptFile->start();
        setWindowModified(true);
    }
    else
    {
        encryptDecryptFileWithPass->setInputFileName(ui->inputLineEdit->text());
        encryptDecryptFileWithPass->setOutputFileName(ui->outputLineEdit->text());
        encryptDecryptFileWithPass->setPassword(ui->keyLineEdit->text());

        checkAll();

        encryptDecryptFileWithPass->start();
        setWindowModified(true);
    }
}

void FileEncryptDecryptDialog::setKeyToFile(bool toggled)
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

        disconnect(encryptDecryptFileWithPass, SIGNAL(fileSize(int)), ui->progressBar, SLOT(setMaximum(int)));
        disconnect(encryptDecryptFileWithPass, SIGNAL(madeProgress(int)), ui->progressBar, SLOT(setValue(int)));
        disconnect(encryptDecryptFileWithPass, SIGNAL(successfullyFinished()), this, SLOT(finished()));
        disconnect(this, SIGNAL(finallyStop()), encryptDecryptFileWithPass, SLOT(stop()));
        disconnect(encryptDecryptFileWithPass, SIGNAL(canceled()), this, SLOT(canceled()));
        disconnect(encryptDecryptFileWithPass, SIGNAL(errorsOccurred(int,QString,QString)), this, SLOT(showErrors(int,QString,QString)));

        connect(encryptDecryptFile, SIGNAL(fileSize(int)), ui->progressBar, SLOT(setMaximum(int)), Qt::UniqueConnection);
        connect(encryptDecryptFile, SIGNAL(madeProgress(int)), ui->progressBar, SLOT(setValue(int)), Qt::UniqueConnection);
        connect(encryptDecryptFile, SIGNAL(successfullyFinished()), this, SLOT(finished()), Qt::UniqueConnection);
        connect(this, SIGNAL(finallyStop()), encryptDecryptFile, SLOT(stop()), Qt::UniqueConnection);
        connect(encryptDecryptFile, SIGNAL(canceled()), this, SLOT(canceled()), Qt::UniqueConnection);
        connect(encryptDecryptFile, SIGNAL(errorsOccurred(int,QString,QString)), this, SLOT(showErrors(int,QString,QString)), Qt::UniqueConnection);
    }
}

void FileEncryptDecryptDialog::setKeyToPassword(bool toggled)
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

        disconnect(encryptDecryptFile, SIGNAL(fileSize(int)), ui->progressBar, SLOT(setMaximum(int)));
        disconnect(encryptDecryptFile, SIGNAL(madeProgress(int)), ui->progressBar, SLOT(setValue(int)));
        disconnect(encryptDecryptFile, SIGNAL(successfullyFinished()), this, SLOT(finished()));
        disconnect(this, SIGNAL(finallyStop()), encryptDecryptFile, SLOT(stop()));
        disconnect(encryptDecryptFile, SIGNAL(canceled()), this, SLOT(canceled()));
        disconnect(encryptDecryptFile, SIGNAL(errorsOccurred(int,QString,QString)), this, SLOT(showErrors(int,QString,QString)));

        connect(encryptDecryptFileWithPass, SIGNAL(fileSize(int)), ui->progressBar, SLOT(setMaximum(int)), Qt::UniqueConnection);
        connect(encryptDecryptFileWithPass, SIGNAL(madeProgress(int)), ui->progressBar, SLOT(setValue(int)), Qt::UniqueConnection);
        connect(encryptDecryptFileWithPass, SIGNAL(successfullyFinished()), this, SLOT(finished()), Qt::UniqueConnection);
        connect(this, SIGNAL(finallyStop()), encryptDecryptFileWithPass, SLOT(stop()), Qt::UniqueConnection);
        connect(encryptDecryptFileWithPass, SIGNAL(canceled()), this, SLOT(canceled()), Qt::UniqueConnection);
        connect(encryptDecryptFileWithPass, SIGNAL(errorsOccurred(int,QString,QString)), this, SLOT(showErrors(int,QString,QString)), Qt::UniqueConnection);
    }
}

bool FileEncryptDecryptDialog::stop(int value)
{
    if(value >= 100)
    {
        int i = QMessageBox::warning(this,tr("Encrypt File"), tr("Do you really want to stop the operation?"),
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

void FileEncryptDecryptDialog::finished()
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
    QMessageBox::information(this, tr("Finished"), tr("Successfully finished operation."), QMessageBox::Ok);
    restoreUi();
}

void FileEncryptDecryptDialog::restoreUi()
{
    setWindowModified(false);

    this->setWindowTitle(tr("Encrypt/Decrypt File"));

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

void FileEncryptDecryptDialog::showErrors(int errorCode, QString title, QString text)
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

void FileEncryptDecryptDialog::checkInputFile(QString inputFileName)
{
    if(inputFileName.isEmpty())
    {
        inputFileStatus = FileEncryptDecryptDialog::NonChosen;
        applyCheckToUi();
        return;
    }

    if(inputFileName == ui->outputLineEdit->text())
    {
        inputFileStatus = FileEncryptDecryptDialog::Equal;
        applyCheckToUi();
        return;
    }

    QFile inputFile(inputFileName);
    if(!inputFile.exists())
    {
        inputFileStatus = FileEncryptDecryptDialog::DoesntExist;
    }
    else
    {
        inputFileStatus = FileEncryptDecryptDialog::Exists;
    }
    applyCheckToUi();
}

void FileEncryptDecryptDialog::checkOutputFile(QString outputFileName)
{
    if(outputFileName.isEmpty())
    {
        outputFileStatus = FileEncryptDecryptDialog::NonChosen;
        applyCheckToUi();
        return;
    }

    if(outputFileName == ui->inputLineEdit->text())
    {
        outputFileStatus = FileEncryptDecryptDialog::Equal;
        applyCheckToUi();
        return;
    }

    QFile outputFile(outputFileName);
    if(!outputFile.exists())
    {
        outputFileStatus = FileEncryptDecryptDialog::DoesntExist;
    }
    else
    {
        outputFileStatus = FileEncryptDecryptDialog::Exists;
    }
    applyCheckToUi();
}

void FileEncryptDecryptDialog::checkKeyFile(QString keyFileName)
{
    if(keyFileName.isEmpty())
    {
        keyFileStatus = FileEncryptDecryptDialog::NonChosen;
        applyCheckToUi();
        return;
    }

    QFile keyFile(keyFileName);
    if(!keyFile.exists())
    {
        keyFileStatus = FileEncryptDecryptDialog::DoesntExist;
    }
    else
    {
        keyFileStatus = FileEncryptDecryptDialog::Exists;
    }
    applyCheckToUi();
}

void FileEncryptDecryptDialog::checkPassword(QString password)
{
    if(password.isEmpty() && ui->repeatPasswordEdit->text().isEmpty())
    {
        passwordStatus = FileEncryptDecryptDialog::NonGiven;
        applyCheckToUi();
        return;
    }
    if(password == ui->repeatPasswordEdit->text())
        passwordStatus = FileEncryptDecryptDialog::Equal;
    else
        passwordStatus = FileEncryptDecryptDialog::NotEqual;

    applyCheckToUi();
}

void FileEncryptDecryptDialog::checkRepeatedPassword(QString repeatedPassword)
{
    if(repeatedPassword.isEmpty() && ui->keyLineEdit->text().isEmpty())
    {
        passwordStatus = FileEncryptDecryptDialog::NonGiven;
        applyCheckToUi();
        return;
    }
    if(repeatedPassword == ui->keyLineEdit->text())
        passwordStatus = FileEncryptDecryptDialog::Equal;
    else
        passwordStatus = FileEncryptDecryptDialog::NotEqual;

    applyCheckToUi();
}

void FileEncryptDecryptDialog::checkAll()
{
    checkInputFile(ui->inputLineEdit->text());
    checkOutputFile(ui->outputLineEdit->text());
    if(keyIsFile)
        checkKeyFile(ui->keyLineEdit->text());
    else
    {
        checkPassword(ui->keyLineEdit->text());
        checkRepeatedPassword(ui->repeatPasswordEdit->text());
    }
}

void FileEncryptDecryptDialog::applyCheckToUi()
{
    if(inputFileStatus == FileEncryptDecryptDialog::NonChosen)
    {
        ui->inputFileStatusLabel->setText(tr("No file chosen!"));
        ui->inputFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
    }
    else if(inputFileStatus == FileEncryptDecryptDialog::DoesntExist)
    {
        ui->inputFileStatusLabel->setText(tr("Doesn't exist!"));
        ui->inputFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
    }
    else if(inputFileStatus == FileEncryptDecryptDialog::Exists)
    {
        ui->inputFileStatusLabel->setText(tr("Exists"));
        ui->inputFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(0, 170, 0);"));
    }
    else if(inputFileStatus == FileEncryptDecryptDialog::Equal)
    {
        ui->inputFileStatusLabel->setText(tr("Already output file!"));
        ui->inputFileStatusLabel->setStyleSheet(QString::fromAscii("color:rgb(170, 0, 0)"));
    }

    if(outputFileStatus == FileEncryptDecryptDialog::NonChosen)
    {
        ui->outputFileStatusLabel->setText(tr("No file chosen!"));
        ui->outputFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
    }
    else if(outputFileStatus == FileEncryptDecryptDialog::DoesntExist)
    {
        ui->outputFileStatusLabel->setText(tr("Doesn't exist!"));
        ui->outputFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(0, 170, 0);"));
    }
    else if(outputFileStatus == FileEncryptDecryptDialog::Exists)
    {
        ui->outputFileStatusLabel->setText(tr("Exists"));
        ui->outputFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(0, 170, 0);"));
    }
    else if(outputFileStatus == FileEncryptDecryptDialog::Equal)
    {
        ui->outputFileStatusLabel->setText(tr("Already input file!"));
        ui->outputFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
    }

    if(keyIsFile)
    {
        if(keyFileStatus == FileEncryptDecryptDialog::NonChosen)
        {
            ui->keyFileStatusLabel->setText(tr("No file chosen!"));
            ui->keyFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
        }
        else if(keyFileStatus == FileEncryptDecryptDialog::DoesntExist)
        {
            ui->keyFileStatusLabel->setText(tr("Doesn't exist."));
            ui->keyFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
        }
        else if(keyFileStatus == FileEncryptDecryptDialog::Exists)
        {
            ui->keyFileStatusLabel->setText(tr("Exists"));
            ui->keyFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(0, 170, 0);"));
        }

        if(inputFileStatus != FileEncryptDecryptDialog::Exists || outputFileStatus == FileEncryptDecryptDialog::NonChosen || keyFileStatus != FileEncryptDecryptDialog::Exists)
        {
            ui->startButton->setEnabled(false);
        }
        else if(inputFileStatus == FileEncryptDecryptDialog::Exists && (outputFileStatus == FileEncryptDecryptDialog::Exists || outputFileStatus == FileEncryptDecryptDialog::DoesntExist) && keyFileStatus == FileEncryptDecryptDialog::Exists)
        {
            ui->startButton->setEnabled(true);
        }
    }
    else if(!keyIsFile)
    {
        if(passwordStatus == FileEncryptDecryptDialog::NonGiven)
        {
            ui->keyFileStatusLabel->setText(tr("Non given!"));
            ui->keyFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
        }
        else if(passwordStatus == FileEncryptDecryptDialog::NotEqual)
        {
            ui->keyFileStatusLabel->setText(tr("Not equal!"));
            ui->keyFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
        }
        else if(passwordStatus == FileEncryptDecryptDialog::Equal)
        {
            ui->keyFileStatusLabel->setText(tr("Equal"));
            ui->keyFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(0, 170, 0);"));
        }

        if(inputFileStatus != FileEncryptDecryptDialog::Exists || outputFileStatus == FileEncryptDecryptDialog::NonChosen || passwordStatus != FileEncryptDecryptDialog::Equal)
        {
            ui->startButton->setEnabled(false);
        }
        else if(inputFileStatus == FileEncryptDecryptDialog::Exists && (outputFileStatus == FileEncryptDecryptDialog::Exists || outputFileStatus == FileEncryptDecryptDialog::DoesntExist) && passwordStatus == FileEncryptDecryptDialog::Equal)
        {
            ui->startButton->setEnabled(true);
        }
    }
}

void FileEncryptDecryptDialog::browseInputFile()
{
    QString home(QDir::homePath());
    home = QDir::toNativeSeparators(home);

    ui->inputLineEdit->setText(QFileDialog::getOpenFileName(this,
                                                            tr("Open Input File"),
                                                            home));
}

void FileEncryptDecryptDialog::browseOutputFile()
{
    QString home(QDir::homePath());
    home = QDir::toNativeSeparators(home);

    ui->outputLineEdit->setText(QFileDialog::getSaveFileName(this,
                                                             tr("Save Output File"),
                                                             home));
}

void FileEncryptDecryptDialog::browseKeyFile()
{
    QString home(QDir::homePath());
    home = QDir::toNativeSeparators(home);

    ui->keyLineEdit->setText(QFileDialog::getOpenFileName(this,
                                                          tr("Open Key File"),
                                                          home));
}

void FileEncryptDecryptDialog::canceled()
{
    QMessageBox::information(this, tr("Canceled"), tr("Canceled by user."), QMessageBox::Ok);
    restoreUi();
}


void FileEncryptDecryptDialog::updateProgressTitle()
{
    if(ui->progressBar->text() != QString::fromAscii("0%"))
        this->setWindowTitle(tr("Encrypt/Decrypt File [%1]").arg(ui->progressBar->text()));
}

void FileEncryptDecryptDialog::closeEvent(QCloseEvent *event)
{
    if(isWindowModified())
    {
        if(stop(100))
            event->accept();
        else
            event->ignore();
    }
}
