#ifndef CHECKFORUPDATESDIALOG_H
#define CHECKFORUPDATESDIALOG_H

#include <QDialog>

namespace Ui {
    class CheckForUpdatesDialog;
}

class CheckForUpdatesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckForUpdatesDialog(QWidget *parent = 0);
    ~CheckForUpdatesDialog();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

private:
    Ui::CheckForUpdatesDialog *ui;
};

#endif // CHECKFORUPDATESDIALOG_H
