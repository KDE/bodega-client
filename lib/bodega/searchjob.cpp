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

#include "searchjob.h"

#include "session.h"
#include "globals.h"

#include <QDebug>

using namespace Bodega;

class SearchJob::Private
{
public:
    Private()
        : moreAssets(false)
    {}

    void init(SearchJob *q, const QString &id, const QUrl &url);
    void parseChannels(const QVariantMap &result);
    void parseAssets(const QVariantMap &result);
    SearchJob *q;
    QString searchText;
    QList<ChannelInfo> channels;
    QList<AssetInfo> assets;
    bool moreAssets;
    int offset;
    int pageSize;
};

void SearchJob::Private::init(SearchJob *parent,
                              const QString &text,
                              const QUrl &url)
{
    q = parent;
    searchText = text;

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

void SearchJob::Private::parseChannels(const QVariantMap &result)
{
    QVariantList channelsLst = result[QLatin1String("channels")].toList();
    QVariantList::const_iterator itr;
    for (itr = channelsLst.constBegin(); itr != channelsLst.constEnd(); ++itr) {
        ChannelInfo info;
        QVariantMap channel = itr->toMap();
        info.id = channel[QLatin1String("id")].toString();
        info.name = channel[QLatin1String("name")].toString();
        info.description = channel[QLatin1String("description")].toString();
        info.images = q->session()->urlsForImage(
            channel[QLatin1String("image")].toString());

        channels.append(info);
    }
}

void SearchJob::Private::parseAssets(const QVariantMap &result)
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

SearchJob::SearchJob(const QString &text, QNetworkReply *reply,
                     Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, text, url());
}

SearchJob::~SearchJob()
{
    delete d;
}

void SearchJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parseChannels(result);
        d->parseAssets(result);
        d->moreAssets = result[QLatin1String("hasMoreAssets")].toBool();
    }
}

QList<ChannelInfo> SearchJob::channels() const
{
    return d->channels;
}

QList<AssetInfo> SearchJob::assets() const
{
    return d->assets;
}

QString SearchJob::text() const
{
    return d->searchText;
}

bool SearchJob::hasMoreAssets() const
{
    return d->moreAssets;
}

int SearchJob::offset() const
{
    return d->offset;
}

int SearchJob::pageSize() const
{
    return d->pageSize;
}

#include "searchjob.moc"
