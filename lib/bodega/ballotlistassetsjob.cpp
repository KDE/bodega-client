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

#include "ballotlistassetsjob.h"

#include "session.h"

using namespace Bodega;

class BallotListAssetsJob::Private
{
public:
    Private()
        : hasMoreAssets(false)
    {}

    void init(BallotListAssetsJob *q, const QUrl &url);
    void parseBallot(const QVariantMap &result);
    void parseAssets(const QVariantMap &result);
    BallotListAssetsJob *q;
    BallotInfo ballot;
    QList<AssetInfo> assets;
    bool hasMoreAssets;
    int offset;
    int pageSize;
};

void BallotListAssetsJob::Private::init(BallotListAssetsJob *parent,
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

void BallotListAssetsJob::Private::parseBallot(const QVariantMap &result)
{
    QVariantMap ballot = result[QLatin1String("ballot")].toMap();
    BallotInfo info;
    info.id = ballot[QLatin1String("id")].toString();
    info.name = ballot[QLatin1String("name")].toString();
    info.flags = BallotInfo::None;
    if (ballot[QLatin1String("public")].toBool()) {
        info.flags |= BallotInfo::Public;
    }
    if (ballot[QLatin1String("wishlist")].toBool()) {
        info.flags |= BallotInfo::Wishlist;
    }
    parseAssets(ballot);

    this->ballot = info;
}

void BallotListAssetsJob::Private::parseAssets(const QVariantMap &result)
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
        info.path = asset[QLatin1String("path")].toString();
        info.images = q->session()->urlsForImage(
            asset[QLatin1String("image")].toString());
        info.description = asset[QLatin1String("description")].toString();
        info.points = asset[QLatin1String("points")].toInt();
        assets.append(info);
    }
}


BallotListAssetsJob::BallotListAssetsJob(QNetworkReply *reply,
                                 Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

BallotListAssetsJob::~BallotListAssetsJob()
{
    delete d;
}

bool BallotListAssetsJob::hasMoreAssets() const
{
    return d->hasMoreAssets;
}

QList<AssetInfo> BallotListAssetsJob::assets() const
{
    return d->assets;
}

BallotInfo BallotListAssetsJob::ballot() const
{
    return d->ballot;
}

int BallotListAssetsJob::offset() const
{
    return d->offset;
}

int BallotListAssetsJob::pageSize() const
{
    return d->pageSize;
}

void BallotListAssetsJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parseBallot(result);
        d->hasMoreAssets = result[QLatin1String("hasMoreAssets")].toBool();
    }
}


#include "ballotlistassetsjob.moc"
