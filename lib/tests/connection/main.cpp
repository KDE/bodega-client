#include "tester.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>

#include <stdlib.h>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    if (app.arguments().size() != 4) {
        qCritical("You have to specify the userid, password and deviceId");
        exit(1);
    }
    QString userId   = app.arguments()[1];
    QString password = app.arguments()[2];
    QString deviceId = app.arguments()[3];

    Tester *tester = new Tester();
    tester->signOn(userId, password, deviceId);

    return app.exec();
}
