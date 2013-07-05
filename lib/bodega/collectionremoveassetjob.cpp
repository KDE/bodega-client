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

#include "collectionremoveassetjob.h"

using namespace Bodega;

class collectionRemoveAssetJob::Private
{
public:
    Private()
    {}

    void init(collectionRemoveAssetJob *q, const QUrl &url);
    void parsecollection(const QVariantMap &result);
    collectionRemoveAssetJob *q;
    collectionInfo collection;
};

void collectionRemoveAssetJob::Private::init(collectionRemoveAssetJob *parent,
                                      const QUrl &url)
{
    q = parent;
}

void collectionRemoveAssetJob::Private::parsecollection(const QVariantMap &result)
{
    QVariantMap collection = result[QLatin1String("collection")].toMap();
    collectionInfo info;
    info.id = collection[QLatin1String("id")].toString();
    info.name = collection[QLatin1String("name")].toString();
    info.flags = collectionInfo::None;
    if (collection[QLatin1String("public")].toBool()) {
        info.flags |= collectionInfo::Public;
    }
    if (collection[QLatin1String("wishlist")].toBool()) {
        info.flags |= collectionInfo::Wishlist;
    }

    this->collection = info;
}

collectionRemoveAssetJob::collectionRemoveAssetJob(QNetworkReply *reply,
                                 Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

collectionRemoveAssetJob::~collectionRemoveAssetJob()
{
    delete d;
}

collectionInfo collectionRemoveAssetJob::collection() const
{
    return d->collection;
}

void collectionRemoveAssetJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parsecollection(result);
    }
}

#include "collectionremoveassetjob.moc"
