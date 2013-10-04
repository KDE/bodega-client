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

#include "assetjob.h"

#include "session.h"

#include <QDebug>

#include <iostream>
namespace Bodega
{

class AssetJob::Private
{
public:
    Private()
    {}
    void init(AssetJob *q, const QString &id, const QUrl &url);
    void parseAsset(const QVariantMap &result);
    void parseChangeLog(const QVariantMap &result);
    void parsePreviews(const QVariantMap &result);
    void parseTags(const QVariantMap &result);
    void parseRatings(const QVariantMap &result);

    AssetJob *q;
    QString id;
    AssetInfo info;
    ChangeLog changeLog;
    QStringList previews;
    Bodega::Tags tags;
    QString contentType;
};

void AssetJob::Private::init(AssetJob *parent,
                             const QString &i,
                             const QUrl &url)
{
    q = parent;
    id = i;
}

void AssetJob::Private::parseAsset(const QVariantMap &result)
{
    QVariantMap asset = result[QLatin1String("asset")].toMap();
    info.id = asset[QLatin1String("id")].toString();
    info.license = asset[QLatin1String("license")].toString();
    info.licenseText = asset[QLatin1String("licenseText")].toString();
    info.partnerId = asset[QLatin1String("partnerId")].toString();
    info.partnerName = asset[QLatin1String("partner")].toString();
    info.name = asset[QLatin1String("name")].toString();
    info.filename = asset[QLatin1String("filename")].toString();
    info.version = asset[QLatin1String("version")].toString();
    info.created = asset[QLatin1String("created")].toDateTime();
    info.images = q->session()->urlsForImage(asset[QLatin1String("image")].toString());
    info.description = asset[QLatin1String("description")].toString();
    info.points = asset[QLatin1String("points")].toInt();
    info.canDownload = asset[QLatin1String("canDownload")].toBool();
    info.size = asset[QLatin1String("size")].toLongLong();
    if (info.size > 1024*1014) {
        info.formatedSize = tr("%1 Mb").arg(((info.size * 100) / (1024 * 1024)) / 100.0);
    } else {
        info.formatedSize = tr("%1 Kb").arg(((info.size * 10) / 1024) / 10.0);
    }

    QVariantList previewList = asset.value(QLatin1String("previews")).value<QVariantList>();
    foreach (const QVariant &item, previewList) {
        const QVariantMap preview = item.value<QVariantMap>();
        QUrl url = q->session()->imageUrls().value(ImagePreviews);

        url.setPath(url.path() + QLatin1String("/") + preview[QLatin1String("path")].toString());
        if (preview[QLatin1String("type")] == QLatin1String("screenshot")) {
            if (preview[QLatin1String("subtype")] == QLatin1String("screen1")) {
                info.previews[ScreenShot1] = url;
            } else if (preview[QLatin1String("subtype")] == QLatin1String("screen2")) {
                info.previews[ScreenShot2] = url;
            }
        } else if (preview[QLatin1String("type")] == QLatin1String("cover")) {
            if (preview[QLatin1String("subtype")] == QLatin1String("front")) {
                info.previews[CoverFront] = url;
            } else if (preview[QLatin1String("subtype")] == QLatin1String("back")) {
                info.previews[CoverBack] = url;
            }
        }
    }
}

void AssetJob::Private::parseChangeLog(const QVariantMap &result)
{
    QVariantMap log = result[QLatin1String("changelog")].toMap();
    QVariantMap::const_iterator itr;

    for (itr = log.constBegin(); itr != log.constEnd(); ++itr) {
        QString version = itr.key();
        QVariantMap changes = itr.value().toMap();
        ChangeLog::Entry entry;
        entry.timestamp = changes[QLatin1String("timestamp")].toDateTime();
        entry.changes = changes[QLatin1String("changes")].toString();
        changeLog.entries.insert(version, entry);
    }
}

void AssetJob::Private::parsePreviews(const QVariantMap &result)
{
    QVariantList vPreviews = result[QLatin1String("previews")].toList();

    foreach (const QVariant &preview, vPreviews) {
        previews.append(preview.toString());
    }
}

void AssetJob::Private::parseRatings(const QVariantMap &result)
{
    QVariantList ratingsList = result[QLatin1String("asset")].toMap()[QLatin1String("ratings")].toList();
    QVariantList::const_iterator itr;
    for (itr = ratingsList.constBegin(); itr != ratingsList.constEnd(); ++itr) {
        AssetInfo::AssetInfoRatings assetInfoRating;
        QVariantMap ratingInfo = itr->toMap();
        assetInfoRating.averageRating = ratingInfo[QLatin1String("averagerating")].toString();
        assetInfoRating.ratingsCount = ratingInfo[QLatin1String("ratingscount")].toString();
        assetInfoRating.attributeId = ratingInfo[QLatin1String("attribute")].toString();

        info.ratings.append(assetInfoRating);
    }
}

void AssetJob::Private::parseTags(const QVariantMap &result)
{
    QVariantList vTags = result[QLatin1String("asset")].toMap()[QLatin1String("tags")].toList();

    foreach(const QVariant &t, vTags) {
        QVariantMap vTag = t.toMap();
        QString key = vTag.keys().first();
        QString value = vTag.values().first().toString();
        if (key == QLatin1String("assetType")) {
            contentType = value;
        }
        tags.insert(key, value);
    }
}

AssetJob::AssetJob(const QString &id, QNetworkReply *reply, Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, id, url());
}

AssetJob::~AssetJob()
{
    delete d;
}

void AssetJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (!failed()) {
        //Parses tags before the asset to be able to access to the hash instead of iterating
        d->parseTags(result);
        d->parseAsset(result);
        d->parseChangeLog(result);
        d->parsePreviews(result);
        d->parseRatings(result);
    }
}

QString AssetJob::assetId() const
{
    return d->id;
}

AssetInfo AssetJob::info() const
{
    return d->info;
}

ChangeLog AssetJob::changeLog() const
{
    return d->changeLog;
}

QStringList AssetJob::previews() const
{
    return d->previews;
}

Bodega::Tags AssetJob::tags() const
{
    return d->tags;
}

QString AssetJob::contentType() const
{
    return d->contentType;;
}

}
#include "assetjob.moc"
