#include "tester.h"

#include <bodega/signonjob.h>

#include <QDebug>

using namespace Bodega;

Tester::Tester()
    : QObject()
{
    session = new Session(this);
    session->setBaseUrl(QUrl(QLatin1String("http://127.0.0.1:3000")));
}

void Tester::signOn(const QString &name,
                    const QString &password,
                    const QString &deviceId)
{
    qDebug() << "signing on with" << name << password << deviceId;
    session->setUserName(name);
    session->setPassword(password);
    session->setDeviceId(deviceId);

    SignOnJob *job = session->signOn();

    connect(job, SIGNAL(signedOn(Bodega::SignOnJob*)), this, SLOT(slotSignedOn()));
    connect(job, SIGNAL(error(Bodega::NetworkJob*,Bodega::Error)), this, SLOT(error(Bodega::NetworkJob*,Bodega::Error)));
}

void Tester::listChannels()
{
}

void Tester::error(Bodega::NetworkJob *job, const Bodega::Error &error)
{
    qDebug() << "FAIL!";
    qDebug() << "     " << error.type() << error.errorId();
    qDebug() << "     " << error.title();
    qDebug() << "     " << error.description();
    exit(1);
}

void Tester::slotSignedOn()
{
    qDebug() << "signed on successfully";
    exit(0);
}
