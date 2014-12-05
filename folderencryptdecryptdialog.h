#ifndef FOLDERENCRYPTDIALOG_H
#define FOLDERENCRYPTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QProgressDialog>

class EncryptDecryptFolder;
class EncryptDecryptFolderWithPass;

namespace Ui {
    class FolderEncryptDecryptDialog;
}

class FolderEncryptDecryptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FolderEncryptDecryptDialog(QWidget *parent = 0);
    ~FolderEncryptDecryptDialog();

signals:
    void finallyStop();
    void setInputFolderPath(QString);
    void setOutputFolderPath(QString);
    void setKeyFileName(QString);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

private:
    Ui::FolderEncryptDecryptDialog *ui;
    void restoreUi();
    void applyCheckToUi();

    int inputFolderStatus;
    int outputFolderStatus;
    int keyFileStatus;
    int passwordStatus;

    bool keyIsFile;

    enum{NonChosen = 0, Exists, DoesntExist, NonGiven, Equal, NotEqual};

    EncryptDecryptFolder *encryptDecryptFolder;
    EncryptDecryptFolderWithPass *encryptDecryptFolderWithPass;


private slots:
    void pressedStartButton();
    void setKeyToFile(bool);
    void setKeyToPassword(bool);
    bool stop(int);
    void finished();
    void showErrors(int, QString, QString);
    void checkInputFolder(QString);
    void checkOutputFolder(QString);
    void checkKeyFile(QString);
    void checkAll();
    void checkPassword(QString);
    void checkRepeatedPassword(QString);
    void browseInputFolder();
    void browseOutputFolder();
    void browseKeyFile();
    void canceled();
    void updateGroupBoxTitle(QString);
};

#endif // FOLDERENCRYPTDIALOG_H
