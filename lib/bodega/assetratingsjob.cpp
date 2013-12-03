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
    QList<AssetRatings> ratings;
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
        AssetRatings info;
        QVariantMap attribute = itr->toMap();
        info.attributeId = attribute[QLatin1String("attribute")].toString();
        info.attributeName = attribute[QLatin1String("attributename")].toString();
        info.personId = attribute[QLatin1String("person")].toString();
        info.personName = attribute[QLatin1String("name")].toString();
        info.rated = QDateTime::fromTime_t(attribute[QLatin1String("rated")].toLongLong());
        QVariantList l = attribute[QLatin1String("ratings")].toList();
        QVariantList::const_iterator it;
        for (it = l.constBegin(); it != l.constEnd(); ++it) {
            Rating r;
            QVariantMap data = it->toMap();
            r.attributeId = data[QLatin1String("attribute")].toString();
            r.attributeName = data[QLatin1String("name")].toString();
            r.rating = data[QLatin1String("rating")].toString();
            info.ratings.append(r);
        }
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

QList<AssetRatings> AssetRatingsJob::ratings() const
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
