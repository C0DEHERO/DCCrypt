#include <QtGui/QApplication>
#include <QString>
#include <QTranslator>
#include <QLocale>
#include "dialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTranslator appTranslator;

    appTranslator.load(QString::fromAscii("dccrypt_") + QLocale::system().name(),
                       qApp->applicationDirPath());
    app.installTranslator(&appTranslator);

    Dialog w;
    w.show();

    return app.exec();
}
