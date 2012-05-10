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

#ifndef BODEGA_NETWORKJOB_H
#define BODEGA_NETWORKJOB_H

#include <bodega/error.h>
#include <bodega/globals.h>

#include <QtNetwork/QNetworkReply>
#include <QtCore/QObject>

namespace Bodega {

    class Session;

    class BODEGA_EXPORT NetworkJob : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QUrl url READ url CONSTANT)
        Q_PROPERTY(bool finished READ isFinished NOTIFY finishedChanged)
        Q_PROPERTY(bool failed READ failed NOTIFY failedChanged)
        Q_PROPERTY(bool authSuccess READ authSuccess NOTIFY authSuccessChanged)
        Q_PROPERTY(QString deviceId READ deviceId NOTIFY deviceIdChanged)
        Q_PROPERTY(int points READ points NOTIFY pointsChanged)
        Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
        Q_PROPERTY(QVariantMap parsedJson READ parsedJson NOTIFY parsedJsonChanged)
        Q_PROPERTY(Bodega::Error error READ error NOTIFY errorChanged)

    public:
        NetworkJob(QNetworkReply *reply, Session *parent, bool parseResponse = true);
        ~NetworkJob();

        Session *session() const;

        QNetworkReply *reply() const;

        QUrl url() const;

        bool isFinished() const;

        bool authSuccess() const;

        QString deviceId() const;

        int points() const;

        qreal progress() const;

        bool failed() const;
        Error error() const;

        bool isJsonResponse() const;
        QVariantMap parsedJson() const;

    Q_SIGNALS:
        //FIXME job seems to be readable from qml only if the parameter is a
        //  QObject * instead of a NetworkJob * (same thing for error)
        void jobError(Bodega::NetworkJob *job, const Bodega::Error &error);
        void jobFinished(Bodega::NetworkJob *job);
        void progressChanged(qreal progress);
        void parsedJsonChanged(const QVariantMap &parsedJson);
        void finishedChanged(bool finished);
        void failedChanged(bool failed);
        void authSuccessChanged(bool authSuccess);
        void deviceIdChanged(const QString &deviceId);
        void pointsChanged(int points);
        void errorChanged(const Error &error);

    protected:
        virtual void netError(QNetworkReply::NetworkError code,
                              const QString &msg);
        virtual void netFinished(const QVariantMap &jsonMap);
        virtual void downloadFinished(const QString &filename);

        void parseCommon(const QVariantMap &jsonMap);
        void parseErrors(const QVariantMap &jsonMap);
        //For errors not related to network
        void setError(const Bodega::Error &error);
        void setFinished();

    private:
        class Private;
        Private * const d;

        Q_PRIVATE_SLOT(d, void netError(QNetworkReply::NetworkError code));
        Q_PRIVATE_SLOT(d, void readFromNetwork());
        Q_PRIVATE_SLOT(d, void netFinished());
        Q_PRIVATE_SLOT(d, void downloadProgress(qint64 bytesReceived, qint64 bytesTotal));
    };
}

#endif
