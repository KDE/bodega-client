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

#include "assetratingsjob.h"
#include <QDebug>

using namespace Bodega;

class AssetRatingsJob::Private
{
public:
    Private()
    {}

    void init(AssetRatingsJob *q, const QUrl &url);
    void parseRatings(const QVariantMap &result);
    AssetRatingsJob *q;
    QList<Ratings> ratings;
};

void AssetRatingsJob::Private::init(AssetRatingsJob *parent,
                                   const QUrl &url)
{
    q = parent;
}

void AssetRatingsJob::Private::parseRatings(const QVariantMap &result)
{
    QVariantList ratingsList = result[QLatin1String("ratings")].toList();
    QVariantList::const_iterator itr;
    for (itr = ratingsList.constBegin(); itr != ratingsList.constEnd(); ++itr) {
        Ratings info;
        QVariantMap attribute = itr->toMap();
        info.attributeId = attribute[QLatin1String("attribute")].toString();
        info.rating = attribute[QLatin1String("rating")].toString();
        info.personId = attribute[QLatin1String("person")].toString();
        info.date = attribute[QLatin1String("date_part")].toString();
        ratings.append(info);
    }
}

AssetRatingsJob::AssetRatingsJob(QNetworkReply *reply,
                                 Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

AssetRatingsJob::~AssetRatingsJob()
{
    delete d;
}

QList<Ratings> AssetRatingsJob::ratings() const
{
    return d->ratings;
}

void AssetRatingsJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parseRatings(result);
    }
}

#include "assetratingsjob.moc"
