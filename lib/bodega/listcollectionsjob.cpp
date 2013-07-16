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

#include "listcollectionsjob.h"

using namespace Bodega;

class ListCollectionsJob::Private
{
public:
    Private()
        : hasMorecollections(false)
    {}

    void init(ListCollectionsJob *q, const QUrl &url);
    void parsecollections(const QVariantMap &result);
    ListCollectionsJob *q;
    QList<CollectionInfo> collections;
    bool hasMorecollections;
    int offset;
    int pageSize;
};

void ListCollectionsJob::Private::init(ListCollectionsJob *parent,
                                   const QUrl &url)
{
    q = parent;
    QString offsetStr = url.queryItemValue(
        QLatin1String("offset"));
    QString pageSizeStr = url.queryItemValue(
        QLatin1String("pageSize"));

    if (offsetStr.isEmpty())
        offset = -1;
    else
        offset = offsetStr.toInt();

    if (pageSizeStr.isEmpty())
        pageSize = -1;
    else
        pageSize = pageSizeStr.toInt();
}

void ListCollectionsJob::Private::parsecollections(const QVariantMap &result)
{
    QVariantList collectionsLst = result[QLatin1String("collections")].toList();
    QVariantList::const_iterator itr;
    for (itr = collectionsLst.constBegin(); itr != collectionsLst.constEnd(); ++itr) {
        CollectionInfo info;
        QVariantMap collection = itr->toMap();
        info.id = collection[QLatin1String("id")].toString();
        info.name = collection[QLatin1String("name")].toString();
        info.flags = CollectionInfo::None;
        if (collection[QLatin1String("public")].toBool()) {
            info.flags |= CollectionInfo::Public;
        }
        if (collection[QLatin1String("wishlist")].toBool()) {
            info.flags |= CollectionInfo::Wishlist;
        }
        collections.append(info);
    }
}

ListCollectionsJob::ListCollectionsJob(QNetworkReply *reply,
                               Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

ListCollectionsJob::~ListCollectionsJob()
{
    delete d;
}

QList<CollectionInfo> ListCollectionsJob::collections() const
{
    return d->collections;
}

bool ListCollectionsJob::hasMorecollections() const
{
    return d->hasMorecollections;
}

int ListCollectionsJob::offset() const
{
    return d->offset;
}

int ListCollectionsJob::pageSize() const
{
    return d->pageSize;
}

void ListCollectionsJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parsecollections(result);
        d->hasMorecollections = result[QLatin1String("hasMorecollections")].toBool();
    }
}

#include "listcollectionsjob.moc"
