/*
 *   Copyright 2013 Coherent Theory LLC
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

#include "assetbriefsjob.h"

#include "globals.h"
#include "session.h"

namespace Bodega {

class AssetBriefsJob::Private
{
public:
    QList<AssetInfo> assets;
};

AssetBriefsJob::AssetBriefsJob(QNetworkReply *reply, Session *parent)
    : NetworkJob(reply, parent, true),
      d(new Private)
{
}

AssetBriefsJob::~AssetBriefsJob()
{
    delete d;
}

QList<AssetInfo> AssetBriefsJob::assets() const
{
    return d->assets;
}

void AssetBriefsJob::netFinished(const QVariantMap &jsonMap)
{
    QVariantList assets = jsonMap[QLatin1String("assets")].toList();
    foreach (const QVariant &v, assets) {
        AssetInfo info;
        const QVariantMap asset = v.toMap();
        info.id = asset[QLatin1String("id")].toString();
        info.license = asset[QLatin1String("license")].toString();
        info.licenseText = asset[QLatin1String("licenseText")].toString();
        info.partnerId = asset[QLatin1String("partnerId")].toString();
        info.partnerName = asset[QLatin1String("partner")].toString();
        info.name = asset[QLatin1String("name")].toString();
        info.filename = asset[QLatin1String("filename")].toString();
        info.version = asset[QLatin1String("version")].toString();
        info.created = asset[QLatin1String("created")].toDateTime();
        info.images = session()->urlsForImage(asset[QLatin1String("image")].toString());
        info.description = asset[QLatin1String("description")].toString();
        d->assets.append(info);
    }
}

}

#include "assetbriefsjob.moc"

