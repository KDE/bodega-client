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


#ifndef SESSION_PRIVATE
#define SESSION_PRIVATE

#include "session.h"

namespace Bodega {

class Session::Private {
public:
    Private(Session *parent);

    QByteArray jsonPath(const char *cmd, bool includeBasePath=true) const
    {
        QString str;
        if (includeBasePath)
            str = QString::fromLatin1("%1%2")
                  .arg(QLatin1String("/bodega/v1/json"))
                  .arg(QLatin1String(cmd));
        else
            str = QLatin1String(cmd);
        return str.toUtf8();
    }
    QByteArray jsonPath(const QString &str, bool includeBasePath=true) const
    {
        return jsonPath(str.toUtf8().constData(), includeBasePath);
    }

    void jobConnect(NetworkJob *job)
    {
        connect(job, SIGNAL(jobFinished(Bodega::NetworkJob*)),
                q, SLOT(jobFinished(Bodega::NetworkJob*)));
    }

    void signOnFinished(Bodega::SignOnJob *job);
    void jobFinished(Bodega::NetworkJob *job);
    QNetworkReply *get(const QUrl &url);

    Session *q;
    QNetworkAccessManager *netManager;
    QUrl baseUrl;
    QString userName;
    QString password;
    QString deviceId;
    bool authenticated;
    int points;
    QMap<ImageUrl, QUrl> imageUrls;
};

}

#endif
