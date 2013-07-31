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

#include "ratingattributesjob.h"
#include <QDebug>

using namespace Bodega;

class RatingAttributesJob::Private
{
public:
    Private()
    {}

    void init(RatingAttributesJob *q, const QUrl &url);
    void parseAttributes(const QVariantMap &result);
    RatingAttributesJob *q;
    QList<RatingAttributes> ratingAttributes;
};

void RatingAttributesJob::Private::init(RatingAttributesJob *parent,
                                   const QUrl &url)
{
    q = parent;
}

void RatingAttributesJob::Private::parseAttributes(const QVariantMap &result)
{
    QVariantList attributesList = result[QLatin1String("ratingAttributes")].toList();
    QVariantList::const_iterator itr;
    for (itr = attributesList.constBegin(); itr != attributesList.constEnd(); ++itr) {
        RatingAttributes info;
        QVariantMap attribute = itr->toMap();
        info.name = attribute[QLatin1String("name")].toString();
        info.lowDesc = attribute[QLatin1String("lowdesc")].toString();
        info.highDesc = attribute[QLatin1String("highdesc")].toString();
        info.assetType = attribute[QLatin1String("assetType")].toString();
        ratingAttributes.append(info);
    }
}

RatingAttributesJob::RatingAttributesJob(QNetworkReply *reply,
                                 Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

RatingAttributesJob::~RatingAttributesJob()
{
    delete d;
}

QList<RatingAttributes> RatingAttributesJob::ratingAttributes() const
{
    return d->ratingAttributes;
}

void RatingAttributesJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parseAttributes(result);
    }
}

#include "ratingattributesjob.moc"
