#ifndef DUMMYSERVER_H
#define DUMMYSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QString>
#include <QByteArray>
#include <QSettings>
#include <QList>

#define DUMMY_PORT 31415

class DummyHttpServer : public QTcpServer
{
    Q_OBJECT
public:
    DummyHttpServer(const QList<QSettings*> &settings);

public slots:
    void respond();
    void handleReadyRead();

private:
    QByteArray createReply(const QByteArray &path) const;
    QByteArray contentForPath(const QByteArray &path) const;

private:
    QHash<QByteArray, QString> fileMap;
};

#endif
