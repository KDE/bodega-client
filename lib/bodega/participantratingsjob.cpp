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

#include "participantratingsjob.h"
#include <QDebug>

using namespace Bodega;

class ParticipantRatingsJob::Private
{
public:
    Private()
    {}

    void init(ParticipantRatingsJob *q, const QUrl &url);
    void parseRatings(const QVariantMap &result);
    ParticipantRatingsJob *q;
    QList<ParticipantRatings> ratings;
};

void ParticipantRatingsJob::Private::init(ParticipantRatingsJob *parent,
                                   const QUrl &url)
{
    q = parent;
}

void ParticipantRatingsJob::Private::parseRatings(const QVariantMap &result)
{
    QVariantList ratingsList = result[QLatin1String("ratings")].toList();
    QVariantList::const_iterator itr;
    for (itr = ratingsList.constBegin(); itr != ratingsList.constEnd(); ++itr) {
        ParticipantRatings info;
        QVariantMap attribute = itr->toMap();
        info.rated = attribute[QLatin1String("rated")].toString();
        info.assetId = attribute[QLatin1String("asset")].toString();
        info.assetName = attribute[QLatin1String("name")].toString();
        info.assetVersion = attribute[QLatin1String("version")].toString();
        info.assetDescription = attribute[QLatin1String("description")].toString();
        QVariantList l = attribute[QLatin1String("ratings")].toList();
        QVariantList::const_iterator it;
        for (it = l.constBegin(); it != l.constEnd(); ++it) {
            ParticipantRatings::Ratings r;
            QVariantMap data = it->toMap();
            r.attributeId = data[QLatin1String("attribute")].toString();
            r.attributeName = data[QLatin1String("name")].toString();
            r.rating = data[QLatin1String("rating")].toString();
            info.ratings.append(r);
        }
        ratings.append(info);
    }
}

ParticipantRatingsJob::ParticipantRatingsJob(QNetworkReply *reply,
                                 Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

ParticipantRatingsJob::~ParticipantRatingsJob()
{
    delete d;
}

QList<ParticipantRatings> ParticipantRatingsJob::ratings() const
{
    return d->ratings;
}

void ParticipantRatingsJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parseRatings(result);
    }
}

#include "participantratingsjob.moc"
