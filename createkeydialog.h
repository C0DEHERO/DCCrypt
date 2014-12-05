#ifndef CREATEKEYDIALOG_H
#define CREATEKEYDIALOG_H

#include <QDialog>

class CreateKey;

namespace Ui {
    class CreateKeyDialog;
}

class CreateKeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateKeyDialog(QWidget *parent = 0);
    ~CreateKeyDialog();

signals:
    void setOutputFileName(QString);
    void finallyStop();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

private:
    Ui::CreateKeyDialog *ui;

    void restoreUi();

    bool sizeFromFile;

    int inputFileStatus;
    int sizeFormat;

    enum{NonChosen = 0, Exists, DoesntExist};
    enum{Bytes = 0, KB, MB, GB};

    CreateKey *createKey;

private slots:
    void applyCheckToUi();
    void pressedStartButton();
    void setSizeToManualInput(bool);
    void setSizeToFile(bool);
    void canceled();
    void updateProgressTitle();
    void browseInputFile();
    void browseOutputFile();
    bool stop(int);
    void checkInputFile(QString);
    void finished();
    void showErrors(int, QString, QString);
    void gettingRandomNumbers();
    void creatingKey();
    void setSizeFormat(int);
};

#endif // CREATEKEYDIALOG_H
