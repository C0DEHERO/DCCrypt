#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QFile>
#include <QMessageBox>

namespace Ui {
    class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    QPoint position;

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

private:
    Ui::Dialog *ui;
    QApplication *app;

public slots:
    void executeFileEncryptDecrypt();
    void executFolderEncryptDecrypt();
    void executeCreateKey();
    void executeEditSettings();
    void showHelp();
    void executeUpdateAssistant();
    void showAbout();
};

#endif // DIALOG_H
