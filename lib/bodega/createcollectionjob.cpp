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

#include "createcollectionjob.h"

using namespace Bodega;

class CreatecollectionJob::Private
{
public:
    Private()
    {}

    void init(CreatecollectionJob *q, const QUrl &url);
    void parsecollections(const QVariantMap &result);
    CreatecollectionJob *q;
    CollectionInfo collection;
};

void CreatecollectionJob::Private::init(CreatecollectionJob *parent,
                                   const QUrl &url)
{
    q = parent;
}

void CreatecollectionJob::Private::parsecollections(const QVariantMap &result)
{
    QVariantList collectionsLst = result[QLatin1String("collections")].toList();

    Q_ASSERT(collectionsLst.count() == 1);

    QVariantMap collection = collectionsLst[0].toMap();
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

CreatecollectionJob::CreatecollectionJob(QNetworkReply *reply,
                                 Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

CreatecollectionJob::~CreatecollectionJob()
{
    delete d;
}

CollectionInfo CreatecollectionJob::collection() const
{
    return d->collection;
}

void CreatecollectionJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parsecollections(result);
    }
}

#include "createcollectionjob.moc"
