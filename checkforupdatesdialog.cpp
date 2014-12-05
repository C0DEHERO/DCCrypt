#include "checkforupdatesdialog.h"
#include "ui_checkforupdatesdialog.h"
#include <QSettings>
#include <QCloseEvent>

CheckForUpdatesDialog::CheckForUpdatesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CheckForUpdatesDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
}

CheckForUpdatesDialog::~CheckForUpdatesDialog()
{
    delete ui;
}

void CheckForUpdatesDialog::changeEvent(QEvent *e)
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

void CheckForUpdatesDialog::closeEvent(QCloseEvent *event)
{
    event->accept();
}
