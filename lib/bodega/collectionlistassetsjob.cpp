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

#include "collectionlistassetsjob.h"

#include "session.h"

using namespace Bodega;

class collectionListAssetsJob::Private
{
public:
    Private()
        : hasMoreAssets(false)
    {}

    void init(collectionListAssetsJob *q, const QUrl &url);
    void parsecollection(const QVariantMap &result);
    void parseAssets(const QVariantMap &result);
    collectionListAssetsJob *q;
    CollectionInfo collection;
    QList<AssetInfo> assets;
    bool hasMoreAssets;
    int offset;
    int pageSize;
};

void collectionListAssetsJob::Private::init(collectionListAssetsJob *parent,
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

void collectionListAssetsJob::Private::parsecollection(const QVariantMap &result)
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

    parseAssets(collection);
    this->collection = info;
}

void collectionListAssetsJob::Private::parseAssets(const QVariantMap &result)
{
    QVariantList assetsLst = result[QLatin1String("assets")].toList();
    QVariantList::const_iterator itr;
    for (itr = assetsLst.constBegin(); itr != assetsLst.constEnd(); ++itr) {
        AssetInfo info;
        QVariantMap asset = itr->toMap();
        info.id = asset[QLatin1String("id")].toString();
        info.license = asset[QLatin1String("license")].toString();
        info.partnerId = asset[QLatin1String("partnerid")].toString();
        info.partnerName = asset[QLatin1String("partnername")].toString();
        info.name = asset[QLatin1String("name")].toString();
        info.version = asset[QLatin1String("version")].toString();
        info.images = q->session()->urlsForImage(
            asset[QLatin1String("image")].toString());
        info.description = asset[QLatin1String("description")].toString();
        info.points = asset[QLatin1String("points")].toInt();
        assets.append(info);
    }
}


collectionListAssetsJob::collectionListAssetsJob(QNetworkReply *reply,
                                 Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

collectionListAssetsJob::~collectionListAssetsJob()
{
    delete d;
}

bool collectionListAssetsJob::hasMoreAssets() const
{
    return d->hasMoreAssets;
}

QList<AssetInfo> collectionListAssetsJob::assets() const
{
    return d->assets;
}

CollectionInfo collectionListAssetsJob::collection() const
{
    return d->collection;
}

int collectionListAssetsJob::offset() const
{
    return d->offset;
}

int collectionListAssetsJob::pageSize() const
{
    return d->pageSize;
}

void collectionListAssetsJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parsecollection(result);
        d->hasMoreAssets = result[QLatin1String("hasMoreAssets")].toBool();
    }
}


#include "collectionlistassetsjob.moc"
