#include "dialog.h"
#include "ui_dialog.h"
#include "fileencryptdecryptdialog.h"
#include "folderencryptdecryptdialog.h"
#include "createkeydialog.h"
#include "settingsdialog.h"
#include "aboutdialog.h"

#include <QSettings>
#include <QCloseEvent>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    setMinimumSize(size());

    connect(ui->encryptDecryptFileButton, SIGNAL(pressed()), this, SLOT(executeFileEncryptDecrypt()));
    connect(ui->encryptDecryptFolderButton, SIGNAL(pressed()), this, SLOT(executFolderEncryptDecrypt()));
    connect(ui->createKeyButton, SIGNAL(pressed()), this, SLOT(executeCreateKey()));
    connect(ui->settingsButton, SIGNAL(pressed()), this, SLOT(executeEditSettings()));
    connect(ui->helpButton, SIGNAL(pressed()), this, SLOT(showHelp()));
    connect(ui->updateButton, SIGNAL(pressed()), this, SLOT(executeUpdateAssistant()));
    connect(ui->aboutButton, SIGNAL(pressed()), this, SLOT(showAbout()));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::changeEvent(QEvent *e)
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

void Dialog::executeFileEncryptDecrypt()
{
    this->hide();
    FileEncryptDecryptDialog dialog(0);
    dialog.exec();
    this->show();
}

void Dialog::executFolderEncryptDecrypt()
{
    this->hide();
    FolderEncryptDecryptDialog dialog(0);
    dialog.exec();
    this->show();
}

void Dialog::executeCreateKey()
{
    this->hide();
    CreateKeyDialog dialog(0);
    dialog.exec();
    this->show();

}

void Dialog::executeEditSettings()
{
    this->hide();
    SettingsDialog dialog(0);
    dialog.exec();
    this->show();
}

void Dialog::showHelp()
{

}

void Dialog::executeUpdateAssistant()
{

}

void Dialog::showAbout()
{
    this->hide();
    AboutDialog dialog(0);
    dialog.exec();
    this->show();
}

void Dialog::closeEvent(QCloseEvent *event)
{
    event->accept();
}
