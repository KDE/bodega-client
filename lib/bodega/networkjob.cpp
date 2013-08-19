/*
 *   Copyright 2012 Coherent Theory LLC
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "networkjob.h"

#include "session.h"

#include <qjson/parser.h>

#include <QDebug>
#include <QTemporaryFile>

using namespace Bodega;

class NetworkJob::Private {
public:
    Private(NetworkJob *nj, QNetworkReply *r, bool parseResults);

    void netError(QNetworkReply::NetworkError code);
    void netFinished();
    void readFromNetwork();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    NetworkJob *q;
    QNetworkReply *reply;
    QJson::Parser parser;
    QVariantMap parsedJson;
    bool finished;
    bool failed;
    Error error;
    bool authSuccess;
    bool expectingJson;
    QString storeId;
    int points;
    QTemporaryFile *file;
    qreal progress;
};

NetworkJob::Private::Private(NetworkJob *nj, QNetworkReply *r, bool parseResults)
    : q(nj),
      reply(r),
      finished(false),
      failed(false),
      authSuccess(false),
      points(0),
      expectingJson(parseResults),
      file(0),
      progress(0)
{
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                     q, SLOT(netError(QNetworkReply::NetworkError)));
    QObject::connect(reply, SIGNAL(finished()), q, SLOT(netFinished()));
    QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
                     q, SLOT(downloadProgress(qint64, qint64)));
    if (!expectingJson) {
        // not json ... so we need to save to a file
        file = new QTemporaryFile(q);
        file->open();
        QObject::connect(reply, SIGNAL(readyRead()), q, SLOT(readFromNetwork()));
    }
}

void NetworkJob::Private::netError(QNetworkReply::NetworkError code)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(q->sender());
    QString str = reply ? reply->errorString() : QString();

    q->netError(code, str);
}

void NetworkJob::Private::netFinished()
{
    if (reply->error() == QNetworkReply::NoError) {
        if (expectingJson) {
            const QByteArray data = reply->readAll();
            bool ok = false;
            parsedJson = parser.parse(data, &ok).toMap();
            //qDebug() << "response from" << reply->url() << "is" << parsedJson;
            if (ok) {
                q->netFinished(parsedJson);
                emit q->parsedJsonChanged(parsedJson);
            } else {
                this->error = Error(Error::Parsing,
                        QLatin1String("nj/01"),
                        tr("Invalid response from the server"),
                        tr("Server has sent an invalid response."));
                failed = true;
                emit q->jobError(q, this->error);
                emit q->failedChanged(true);
            }

            q->setFinished();
        } else {
            file->close();
            //qDebug() << "download finished to" << file->fileName();
            q->downloadFinished(file->fileName());
        }
    } else {
        //qDebug() << "ERRROR!!!!!" << reply->error();
        q->setFinished();
    }
}

void NetworkJob::Private::readFromNetwork()
{
    file->write(reply->readAll());
    if (file->error() != QFile::NoError) {
        this->error = Error(Error::Network,
                            QLatin1String("ij/01"),
                            tr("Download error"),
                            tr("Error downloading the asset."));
        failed = true;
        emit q->jobError(q, this->error);
        emit q->failedChanged(true);
    }
}

void NetworkJob::Private::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesReceived > 0 && bytesTotal > 0) {
        q->setProgress((qreal)1.0 / qreal(bytesTotal / bytesReceived));
    }
}


NetworkJob::NetworkJob(QNetworkReply *reply, Session *parent, bool parseResults)
    : QObject(parent),
      d(new NetworkJob::Private(this, reply, parseResults))
{
}

NetworkJob::~NetworkJob()
{
    delete d;
}

QNetworkReply * NetworkJob::reply() const
{
    return d->reply;
}

bool NetworkJob::failed() const
{
    return d->failed;
}

void NetworkJob::netError(QNetworkReply::NetworkError code,
                          const QString &msg)
{
    QString id = QString::fromLatin1("net/%1").arg(code);
    d->failed = true;
    d->error = Error(Error::Network,
                     id, tr("Network Error"), msg);
    emit jobError(this, d->error);
    emit failedChanged(true);
}

bool NetworkJob::isFinished() const
{
    return d->finished;
}

bool NetworkJob::isJsonResponse() const
{
    return d->expectingJson;
}

QVariantMap NetworkJob::parsedJson() const
{
    return d->parsedJson;
}

QUrl NetworkJob::url() const
{
    return d->reply->url();
}

void NetworkJob::parseErrors(const QVariantMap &jsonMap)
{
    if (!jsonMap.contains(QLatin1String("error"))) {
        return;
    }

    d->failed = true;
    const QVariantMap error = jsonMap[QLatin1String("error")].toMap();
    if (error.isEmpty() && !d->authSuccess) {
        d->error = Error(Error::Authentication,
                         QString::fromLatin1("auth/01"),
                         tr("Authentication Error"),
                         tr("Invalid username or password"));
    } else {
        const Error::ServerCode code = Error::serverCodeId(error.value(QLatin1String("type")).toString());
        d->error = Error(code);
    }

    emit jobError(this, d->error);
    emit failedChanged(true);
}

void NetworkJob::parseCommon(const QVariantMap &result)
{
    const bool authSuccess = d->authSuccess;
    const int points = d->points;
    const QString storeId = storeId;

    d->authSuccess = result[QLatin1String("authStatus")].toBool();
    d->points = qMax(0, d->authSuccess ? result[QLatin1String("points")].toInt() : 0);

    QVariant var = result[QLatin1String("device")];
    // Otherwise we'll make d->storeId == "0" because
    //  QVariant(qulonglong, 0).toString == "0" not a null string
    if (var.toBool()) {
        d->storeId = result[QLatin1String("device")].toString();
    } else {
        d->storeId = QString();
    }

    if (storeId != d->storeId) {
        emit storeIdChanged(d->storeId);
    }

    if (points != d->points) {
        emit pointsChanged(d->points);
    }

    if (authSuccess != d->authSuccess) {
        emit authSuccessChanged(d->authSuccess);
    }

    parseErrors(result);
}

bool NetworkJob::authSuccess() const
{
    return d->authSuccess;
}

QString NetworkJob::storeId() const
{
    return d->storeId;
}

int NetworkJob::points() const
{
    return d->points;
}

qreal NetworkJob::progress() const
{
    return d->progress;
}

void NetworkJob::setProgress(qreal progress)
{
    if (d->progress == progress) {
        return;
    }

    d->progress = progress;
    emit progressChanged(progress);
}

Session *NetworkJob::session() const
{
    return qobject_cast<Session*>(parent());
}

Bodega::Error NetworkJob::error() const
{
    return d->error;
}

void NetworkJob::netFinished(const QVariantMap &jsonMap)
{
    parseCommon(jsonMap);
}

void NetworkJob::setFinished()
{
    d->finished = true;
    emit jobFinished(this);
    emit finishedChanged(true);
}

void NetworkJob::setError(const Bodega::Error &e)
{
    d->error = e;
    if (!e.errorId().isEmpty()) {
        d->failed = true;
        emit jobError(this, d->error);
    }
}

void NetworkJob::downloadFinished(const QString &filename)
{
    Q_UNUSED(filename)
}

#include "networkjob.moc"
