#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QDir>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    setMinimumSize(size());

    settingsFileName = QDir::toNativeSeparators(QString::fromAscii("settings.cfg"));

    loadSettings();

    connect(ui->applyButton, SIGNAL(pressed()), this, SLOT(saveSettings()));

    connect(ui->includeSubdirsOptionCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setIsModified()));
    connect(ui->FollowSymLinksOptionCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setIsModified()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::changeEvent(QEvent *e)
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

void SettingsDialog::readSettings(QString settingsFileName)
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


    bool includeSubdirs = true;
    bool followSymLinks = true;

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
        showErrors(0, tr("Errors Occurred!"), errorString);
    }
    else
    {
        settingsFileStream >> includeSubdirs;
        settingsFileStream >> followSymLinks;
    }

    ui->includeSubdirsOptionCheckBox->setChecked(includeSubdirs);
    ui->FollowSymLinksOptionCheckBox->setChecked(followSymLinks);
}

void SettingsDialog::writeSettings(QString settingsFileName)
{
    QFile settingsFile(settingsFileName);
    if(!settingsFile.open(QFile::WriteOnly))
    {
        showErrors(0, tr("Could not open \"%1\"!").arg(settingsFileName), tr("Could not open \"%1\"!").arg(settingsFileName));
    }

    QDataStream settingsFileStream(&settingsFile);
    settingsFileStream.setVersion(QDataStream::Qt_4_7);


    settingsFileStream << quint32(0x47659436);

    settingsFileStream << ui->includeSubdirsOptionCheckBox->isChecked();

    settingsFileStream << ui->FollowSymLinksOptionCheckBox->isChecked();

    this->setWindowModified(false);
}

void SettingsDialog::saveSettings()
{
    writeSettings(settingsFileName);
    close();
}

void SettingsDialog::loadSettings()
{
    readSettings(settingsFileName);
}

void SettingsDialog::showErrors(int errorCode, QString title, QString text)
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

void SettingsDialog::setIsModified()
{
    setWindowModified(true);
}

bool SettingsDialog::okToContinue()
{
    if(isWindowModified())
    {
        int r = QMessageBox::warning(this, tr("Settings"),
                                     tr("Do you want to save your changes?"),
                                     QMessageBox::Yes | QMessageBox::Default,
                                     QMessageBox::No,
                                     QMessageBox::Cancel | QMessageBox::Escape);
        if(r == QMessageBox::Yes)
            writeSettings(settingsFileName);
        else if(r == QMessageBox::Cancel)
            return false;
    }
    else
        return true;

    return true;
}

void SettingsDialog::closeEvent(QCloseEvent *event)
{
    if(okToContinue())
        event->accept();
    else
        event->ignore();
}
