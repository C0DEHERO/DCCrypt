#include "createkeydialog.h"
#include "ui_createkeydialog.h"
#include "createkey.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QCloseEvent>
#include <QSettings>
#include <cstdlib>

CreateKeyDialog::CreateKeyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateKeyDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    setMinimumSize(size());

    createKey = new CreateKey;

    sizeFromFile = false;

    sizeFormat = CreateKeyDialog::Bytes;


    ui->progressGroupBox->hide();

    ui->startButton->setEnabled(false);

    ui->browseInputButton->hide();
    ui->inputLineEdit->hide();
    ui->inputFileStatusLabel->hide();

    connect(ui->startButton, SIGNAL(pressed()), this, SLOT(pressedStartButton()));
    connect(ui->stopSlider, SIGNAL(valueChanged(int)), this, SLOT(stop(int)));

    connect(ui->inputLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkInputFile(QString)));
    connect(ui->outputLineEdit, SIGNAL(textChanged(QString)), this, SLOT(applyCheckToUi()));

    connect(ui->sizeFormatComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setSizeFormat(int)));

    connect(ui->browseInputButton, SIGNAL(pressed()), this, SLOT(browseInputFile()));
    connect(ui->browseOutputButton, SIGNAL(pressed()), this, SLOT(browseOutputFile()));

    connect(ui->manuallyRadioButton, SIGNAL(toggled(bool)), this, SLOT(setSizeToManualInput(bool)), Qt::UniqueConnection);
    connect(ui->fromFileRadioButton, SIGNAL(toggled(bool)), this, SLOT(setSizeToFile(bool)), Qt::UniqueConnection);

    connect(createKey, SIGNAL(madeProgress(int)), ui->progressBar, SLOT(setValue(int)), Qt::UniqueConnection);
    connect(ui->progressBar, SIGNAL(valueChanged(int)), this, SLOT(updateProgressTitle()), Qt::UniqueConnection);

    connect(createKey, SIGNAL(successfullyFinished()), this, SLOT(finished()), Qt::UniqueConnection);

    connect(this, SIGNAL(finallyStop()), createKey, SLOT(stop()), Qt::UniqueConnection);
    connect(createKey, SIGNAL(canceled()), this, SLOT(canceled()), Qt::UniqueConnection);

    connect(createKey, SIGNAL(errorsOccurred(int,QString,QString)), this, SLOT(showErrors(int,QString,QString)), Qt::UniqueConnection);

    connect(createKey, SIGNAL(gettingRandomNumbers()), this, SLOT(gettingRandomNumbers()), Qt::UniqueConnection);
    connect(createKey, SIGNAL(creatingKey()), this, SLOT(creatingKey()), Qt::UniqueConnection);

    checkInputFile(ui->inputLineEdit->text());
    applyCheckToUi();
    setSizeFormat(CreateKeyDialog::Bytes);
}

CreateKeyDialog::~CreateKeyDialog()
{
    delete ui;
}

void CreateKeyDialog::changeEvent(QEvent *e)
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

void CreateKeyDialog::pressedStartButton()
{
    ui->sizeLabel->setEnabled(false);
    ui->inputLineEdit->setEnabled(false);
    ui->browseInputButton->setEnabled(false);
    ui->sizeSpinBox->setEnabled(false);

    ui->outputLabel->setEnabled(false);
    ui->outputLineEdit->setEnabled(false);
    ui->browseOutputButton->setEnabled(false);

    ui->inputSizeLabel->setEnabled(false);
    ui->manuallyRadioButton->setEnabled(false);
    ui->fromFileRadioButton->setEnabled(false);

    ui->sizeFormatComboBox->setEnabled(false);

    ui->startButton->hide();

    createKey->setFinalKeySize(ui->sizeSpinBox->value());
    createKey->setOutputKeyFileName(ui->outputLineEdit->text());

    ui->progressGroupBox->show();

    createKey->setOutputKeyFileName(ui->outputLineEdit->text());
    if(!sizeFromFile)
    {
        if(sizeFormat == CreateKeyDialog::Bytes)
        {
            createKey->setFinalKeySize(ui->sizeSpinBox->value());
            ui->progressBar->setMaximum(ui->sizeSpinBox->value());

            if(ui->sizeSpinBox->value() <= 0)
                ui->progressBar->setMaximum(0);
        }
        else if(sizeFormat == CreateKeyDialog::KB)
        {
            createKey->setFinalKeySize(ui->sizeSpinBox->value()*double(1024));
            ui->progressBar->setMaximum(ui->sizeSpinBox->value()*double(1024));

            if(ui->sizeSpinBox->value()*double(1024) <= 0)
                ui->progressBar->setMaximum(0);
        }
        else if(sizeFormat == CreateKeyDialog::MB)
        {
            createKey->setFinalKeySize(ui->sizeSpinBox->value()*double(1048576));
            ui->progressBar->setMaximum(ui->sizeSpinBox->value()*double(1048576));

            if(ui->sizeSpinBox->value()*double(1048576) <= 0)
                ui->progressBar->setMaximum(0);
        }
        else if(sizeFormat == CreateKeyDialog::GB)
        {
            createKey->setFinalKeySize(ui->sizeSpinBox->value()*double(1073741824));
            ui->progressBar->setMaximum(ui->sizeSpinBox->value()*double(1073741824));

            if(ui->sizeSpinBox->value()*double(1073741824) <= 0)
                ui->progressBar->setMaximum(0);
        }
    }
    else
    {
        QFile inputFile(ui->inputLineEdit->text());
        createKey->setFinalKeySize(inputFile.size());
        ui->progressBar->setMaximum(inputFile.size());
    }

        createKey->start();
        setWindowModified(true);
}

bool CreateKeyDialog::stop(int value)
{
    if(value >= 100)
    {
        int i = QMessageBox::warning(this,tr("Create Key"), tr("Do you really want to stop the operation?"),
                                     QMessageBox::Yes, QMessageBox::No | QMessageBox::Escape);

        if(i == QMessageBox::Yes)
        {
            if(ui->progressBar->maximum() == 0)
            {
                createKey->terminate();
                createKey->wait();
            }
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

void CreateKeyDialog::checkInputFile(QString inputFileName)
{
    if(inputFileName.isEmpty())
    {
        inputFileStatus = CreateKeyDialog::NonChosen;
        applyCheckToUi();
        return;
    }
    else
    {
        QFile inputFile(inputFileName);
        if(inputFile.exists())
        {
            inputFileStatus = CreateKeyDialog::Exists;

            if(sizeFormat == CreateKeyDialog::Bytes)
                ui->sizeSpinBox->setValue(inputFile.size());
            else if(sizeFormat == CreateKeyDialog::KB)
                ui->sizeSpinBox->setValue(double(inputFile.size())/double(1024));
            else if(sizeFormat == CreateKeyDialog::MB)
                ui->sizeSpinBox->setValue(double(inputFile.size())/double(1048576));
            else if(sizeFormat == CreateKeyDialog::GB)
                ui->sizeSpinBox->setValue(double(inputFile.size())/double(1073741824));

            sizeFromFile = true;
        }
        else
            inputFileStatus = CreateKeyDialog::DoesntExist;

        applyCheckToUi();
    }
}

void CreateKeyDialog::browseInputFile()
{
    QString home(QDir::homePath());
    home = QDir::toNativeSeparators(home);

    ui->inputLineEdit->setText(QFileDialog::getOpenFileName(this,
                                                            tr("Open Input File"),
                                                            home));
}

void CreateKeyDialog::browseOutputFile()
{
    QString home(QDir::homePath());
    home = QDir::toNativeSeparators(home);

    ui->outputLineEdit->setText(QFileDialog::getSaveFileName(this,
                                                            tr("Save Output File"),
                                                            home));
}

void CreateKeyDialog::updateProgressTitle()
{
    if(ui->progressBar->text() != QString::fromAscii("0%"))
        this->setWindowTitle(tr("Create Key [%1]").arg(ui->progressBar->text()));
}

void CreateKeyDialog::finished()
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

void CreateKeyDialog::canceled()
{
    QMessageBox::information(this, tr("Canceled"), tr("Canceled by user."), QMessageBox::Ok);
    restoreUi();
}

void CreateKeyDialog::showErrors(int errorCode, QString title, QString text)
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

void CreateKeyDialog::restoreUi()
{
    setWindowModified(false);
    this->setWindowTitle(tr("Create Key"));

    ui->sizeLabel->setEnabled(true);
    ui->inputLineEdit->setEnabled(true);
    ui->browseInputButton->setEnabled(true);
    ui->sizeSpinBox->setEnabled(true);
    ui->sizeFormatComboBox->setEnabled(true);

    ui->outputLabel->setEnabled(true);
    ui->outputLineEdit->setEnabled(true);
    ui->browseOutputButton->setEnabled(true);

    ui->manuallyRadioButton->setEnabled(true);
    ui->fromFileRadioButton->setEnabled(true);

    ui->startButton->show();

    ui->progressGroupBox->hide();
    ui->stopSlider->setValue(0);
    ui->progressBar->setValue(0);
    ui->shutdownCheckBox->setChecked(false);

    ui->progressGroupBox->hide();
}

void CreateKeyDialog::applyCheckToUi()
{

    if(inputFileStatus == CreateKeyDialog::NonChosen)
    {
        ui->inputFileStatusLabel->setText(tr("No file chosen!"));
        ui->inputFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
    }
    else if(inputFileStatus == CreateKeyDialog::DoesntExist)
    {
        ui->inputFileStatusLabel->setText(tr("Doesn't exist!"));
        ui->inputFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(170, 0, 0);"));
    }
    else if(inputFileStatus == CreateKeyDialog::Exists)
    {
        ui->inputFileStatusLabel->setText(tr("Exists"));
        ui->inputFileStatusLabel->setStyleSheet(QString::fromAscii("color: rgb(0, 170, 0);"));
    }

    if((inputFileStatus != CreateKeyDialog::Exists && sizeFromFile) || ui->outputLineEdit->text().isEmpty())
    {
        ui->startButton->setEnabled(false);
    }
    else if((inputFileStatus == CreateKeyDialog::Exists || !sizeFromFile) && !ui->outputLineEdit->text().isEmpty())
    {
        ui->startButton->setEnabled(true);
    }
}

void CreateKeyDialog::setSizeToManualInput(bool toggled)
{
    if(toggled)
    {
        sizeFromFile = false;
        ui->sizeLabel->setText(QString::fromAscii("Size:"));
        ui->inputLineEdit->hide();
        ui->inputFileStatusLabel->hide();
        ui->sizeSpinBox->setEnabled(true);
        ui->browseInputButton->hide();
    }

}

void CreateKeyDialog::setSizeToFile(bool toggled)
{
    if(toggled)
    {
        sizeFromFile = true;
        ui->sizeLabel->setText(QString::fromAscii("File:"));
        ui->inputLineEdit->show();
        ui->inputFileStatusLabel->show();
        ui->sizeSpinBox->setEnabled(false);
        ui->browseInputButton->show();
    }
}

void CreateKeyDialog::gettingRandomNumbers()
{
    ui->progressGroupBox->setTitle(tr("Getting Random Numbers..."));
    ui->progressBar->setMaximum(0);
}

void CreateKeyDialog::creatingKey()
{
    ui->progressGroupBox->setTitle(tr("Creating Key..."));

    if(!sizeFromFile)
    {
        if(sizeFormat == CreateKeyDialog::Bytes)
        {
            createKey->setFinalKeySize(ui->sizeSpinBox->value());
            ui->progressBar->setMaximum(ui->sizeSpinBox->value());

            if(ui->sizeSpinBox->value() <= 0)
                ui->progressBar->setMaximum(0);
        }
        else if(sizeFormat == CreateKeyDialog::KB)
        {
            createKey->setFinalKeySize(ui->sizeSpinBox->value()*double(1024));
            ui->progressBar->setMaximum(ui->sizeSpinBox->value()*double(1024));

            if(ui->sizeSpinBox->value()*double(1024) <= 0)
                ui->progressBar->setMaximum(0);
        }
        else if(sizeFormat == CreateKeyDialog::MB)
        {
            createKey->setFinalKeySize(ui->sizeSpinBox->value()*double(1048576));
            ui->progressBar->setMaximum(ui->sizeSpinBox->value()*double(1048576));

            if(ui->sizeSpinBox->value()*double(1048576) <= 0)
                ui->progressBar->setMaximum(0);
        }
        else if(sizeFormat == CreateKeyDialog::GB)
        {
            createKey->setFinalKeySize(ui->sizeSpinBox->value()*double(1073741824));
            ui->progressBar->setMaximum(ui->sizeSpinBox->value()*double(1073741824));

            if(ui->sizeSpinBox->value()*double(1073741824) <= 0)
                ui->progressBar->setMaximum(0);
        }
    }
    else
    {
        QFile inputFile(ui->inputLineEdit->text());
        createKey->setFinalKeySize(inputFile.size());
        ui->progressBar->setMaximum(inputFile.size());
    }
}

void CreateKeyDialog::setSizeFormat(int format)
{
    sizeFormat = format;

    if(format == CreateKeyDialog::Bytes)
    {
        ui->sizeSpinBox->setMinimum(double(1));
        ui->sizeSpinBox->setDecimals(0);
    }
    else if(format == CreateKeyDialog::KB)
    {
        ui->sizeSpinBox->setMinimum(double(0.1));
        ui->sizeSpinBox->setDecimals(2);
    }
    else if(format == CreateKeyDialog::MB)
    {
        ui->sizeSpinBox->setMinimum(double(0.1));
        ui->sizeSpinBox->setDecimals(2);
    }
    else if(format == CreateKeyDialog::GB)
    {
        ui->sizeSpinBox->setMinimum(double(0.1));
        ui->sizeSpinBox->setDecimals(2);
    }

    checkInputFile(ui->inputLineEdit->text());
}

void CreateKeyDialog::closeEvent(QCloseEvent *event)
{
    if(isWindowModified())
    {
        if(stop(100))
            event->accept();
        else
            event->ignore();
    }
}
