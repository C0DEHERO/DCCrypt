#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

private:
    Ui::SettingsDialog *ui;
    QString settingsFileName;

private slots:
    void writeSettings(QString);
    void readSettings(QString);

    void saveSettings();
    void loadSettings();

    void showErrors(int, QString, QString);

    void setIsModified();

    bool okToContinue();
};

#endif // SETTINGSDIALOG_H
