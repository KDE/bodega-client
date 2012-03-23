#include "dummyserver.h"

#include <QFile>
#include <QStringList>

const char *VERSION = "/bodega/v1";

DummyHttpServer::DummyHttpServer(const QList<QSettings*> &settingsLst)
{
    foreach(QSettings *settings, settingsLst) {
        foreach(QString topGroup, settings->childGroups()) {
            //qDebug()<<"top group is "<<topGroup;
            settings->beginGroup(topGroup);
            foreach(QString test, settings->childGroups()) {
                settings->beginGroup(test);
                QString query = settings->value(QLatin1String("url")).toString();
#if 0
                query = QString::fromLatin1("%1%2")
                        .arg(QLatin1String(VERSION))
                        .arg(query);
#endif
                fileMap.insert(query.toUtf8(),
                               QString::fromLatin1("%1/responses/%2%3.txt")
                               .arg(QString::fromLatin1(SOURCE_DIR))
                               .arg(topGroup)
                               .arg(test));
                settings->endGroup();
            }
            settings->endGroup();
        }
    }

    connect(this, SIGNAL(newConnection()),
            SLOT(respond()));

    listen(QHostAddress::LocalHost, DUMMY_PORT);
}

QByteArray DummyHttpServer::contentForPath(const QByteArray &path) const
{
    //qDebug()<<"Path = "<<path;
    if (!fileMap.contains(path)) {
        qDebug()<<"DummyHttpServer: missing content for = "<<path;
        return QByteArray();
    }

    QFile file(fileMap[path]);
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    return data;
}

QByteArray DummyHttpServer::createReply(const QByteArray &path) const
{
    QByteArray content = contentForPath(path);

    QString temp = QString::fromUtf8(
        "HTTP/1.0 200 OK\r\n"
        "Date: Fri, 07 Sep 2007 12:33:18 GMT\r\n"
        "Expires:\r\n"
        "Connection: close\r\n"
        "Content-Type: text/json; charset=utf-8\r\n"
        "Content-Length: %1\r\n"
        "\r\n%2").arg(content.length()).arg(QString::fromUtf8(content));
    return temp.toUtf8();
}

void DummyHttpServer::respond()
{
    QTcpSocket *clientConnection = nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));
    connect(clientConnection, SIGNAL(readyRead()), SLOT(handleReadyRead()));
}

void DummyHttpServer::handleReadyRead()
{
    QTcpSocket *socket = static_cast<QTcpSocket *>(sender());
    QByteArray data = socket->readAll();

    int start = 4;// "GET "
    int end = data.indexOf("HTTP/") - 1;
    QByteArray path = data.mid(start, end - start);

    QByteArray content = createReply(path);

    socket->write(content);
    socket->disconnectFromHost();
}

#include "dummyserver.moc"
