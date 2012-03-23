#ifndef TESTER_H
#define TESTER_H

#include <bodega/session.h>
#include <QtCore/QObject>

class Tester : public QObject
{
    Q_OBJECT
public:
    Tester();

public slots:
    void signOn(const QString &name,
                const QString &password,
                const QString &deviceId);
    void listChannels();

private slots:
    void error(Bodega::NetworkJob *job, const Bodega::Error &error);
    void slotSignedOn();

private:
    Bodega::Session *session;
};

#endif
