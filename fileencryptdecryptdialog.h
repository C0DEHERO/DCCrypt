#ifndef FILEENCRYPTDIALOG_H
#define FILEENCRYPTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QProgressDialog>

class EncryptDecryptFile;
class EncryptDecryptFileWithPass;

namespace Ui {
    class FileEncryptDecryptDialog;
}

class FileEncryptDecryptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileEncryptDecryptDialog(QWidget *parent = 0);
    ~FileEncryptDecryptDialog();

signals:
    void finallyStop();
    void setInputFileName(QString);
    void setOutputFileName(QString);
    void setKeyFileName(QString);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

private:
    Ui::FileEncryptDecryptDialog *ui;
    void restoreUi();
    void applyCheckToUi();

    int inputFileStatus;
    int outputFileStatus;
    int keyFileStatus;
    int passwordStatus;

    bool keyIsFile;

    enum{NonChosen = 0, Exists, DoesntExist, NonGiven, Equal, NotEqual};

    EncryptDecryptFile *encryptDecryptFile;
    EncryptDecryptFileWithPass *encryptDecryptFileWithPass;


private slots:
    void pressedStartButton();
    void setKeyToFile(bool);
    void setKeyToPassword(bool);
    bool stop(int);
    void finished();
    void showErrors(int, QString, QString);
    void checkInputFile(QString);
    void checkOutputFile(QString);
    void checkKeyFile(QString);
    void checkAll();
    void checkPassword(QString);
    void checkRepeatedPassword(QString);
    void browseInputFile();
    void browseOutputFile();
    void browseKeyFile();
    void canceled();
    void updateProgressTitle();
};

#endif // FILEENCRYPTDIALOG_H
