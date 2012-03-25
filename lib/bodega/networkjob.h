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
        Q_PROPERTY(QUrl url READ url)
        Q_PROPERTY(bool finished READ isFinished)
        Q_PROPERTY(bool failed READ failed)
        Q_PROPERTY(bool authSuccess READ authSuccess)
        Q_PROPERTY(QString deviceId READ deviceId)
        Q_PROPERTY(int points READ points)
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

        bool failed() const;
        Error error() const;

        QVariantMap parsedJson() const;

    Q_SIGNALS:
        //FIXME job seems to be readable from qml only if the parameter is a
        //  QObject * instead of a NetworkJob * (same thing for error)
        void error(Bodega::NetworkJob *job, const Bodega::Error &error);
        void jobFinished(Bodega::NetworkJob *job);

    protected:
        virtual void netError(QNetworkReply::NetworkError code,
                              const QString &msg);
        virtual void netFinished(const QVariantMap &jsonMap);
        virtual void downloadFinished(const QString &filename);

        void parseCommon(const QVariantMap &jsonMap);
        void parseErrors(const QVariantMap &jsonMap);
        void setFinished();

    private:
        class Private;
        Private * const d;

        Q_PRIVATE_SLOT(d, void netError(QNetworkReply::NetworkError code));
        Q_PRIVATE_SLOT(d, void readFromNetwork());
        Q_PRIVATE_SLOT(d, void netFinished());
    };
}

#endif
