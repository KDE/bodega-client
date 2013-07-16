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

#include "collectionaddassetjob.h"

using namespace Bodega;

class collectionAddAssetJob::Private
{
public:
    Private()
    {}

    void init(collectionAddAssetJob *q, const QUrl &url);
    void parsecollection(const QVariantMap &result);
    collectionAddAssetJob *q;
    CollectionInfo collection;
};

void collectionAddAssetJob::Private::init(collectionAddAssetJob *parent,
                                      const QUrl &url)
{
    q = parent;
}

void collectionAddAssetJob::Private::parsecollection(const QVariantMap &result)
{
    QVariantMap collection = result[QLatin1String("collection")].toMap();
    CollectionInfo info;
    info.id = collection[QLatin1String("id")].toString();
    info.name = collection[QLatin1String("name")].toString();
    info.flags = CollectionInfo::None;
    if (collection[QLatin1String("public")].toBool()) {
        info.flags |= CollectionInfo::Public;
    }
    if (collection[QLatin1String("wishlist")].toBool()) {
        info.flags |= CollectionInfo::Wishlist;
    }

    this->collection = info;
}

collectionAddAssetJob::collectionAddAssetJob(QNetworkReply *reply,
                                 Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

collectionAddAssetJob::~collectionAddAssetJob()
{
    delete d;
}

CollectionInfo collectionAddAssetJob::collection() const
{
    return d->collection;
}

void collectionAddAssetJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parsecollection(result);
    }
}

#include "collectionaddassetjob.moc"
