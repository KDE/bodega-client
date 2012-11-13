#include "tester.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>

#include <stdlib.h>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    if (app.arguments().size() != 4) {
        qCritical("You have to specify the userid, password and storeId");
        exit(1);
    }
    QString userId   = app.arguments()[1];
    QString password = app.arguments()[2];
    QString storeId = app.arguments()[3];

    Tester *tester = new Tester();
    tester->signOn(userId, password, storeId);

    return app.exec();
}
