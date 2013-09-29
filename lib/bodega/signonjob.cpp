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

#include "signonjob.h"

#include "session.h"

#include <QDebug>

using namespace Bodega;

class SignOnJob::Private
{
public:
    Private()
    {}
    QMap<ImageUrl, QUrl> imageUrls;
};

SignOnJob::SignOnJob(QNetworkReply *reply, Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
}

SignOnJob::~SignOnJob()
{
    delete d;
}

void SignOnJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        QVariantMap urls = result[QLatin1String("imageUrls")].toMap();
        if (!urls.isEmpty()) {
            QUrl tiny = urls[QLatin1String("tiny")].toUrl();
            QUrl small = urls[QLatin1String("small")].toUrl();
            QUrl medium = urls[QLatin1String("medium")].toUrl();
            QUrl big = urls[QLatin1String("big")].toUrl();
            QUrl large = urls[QLatin1String("large")].toUrl();
            QUrl huge = urls[QLatin1String("huge")].toUrl();
            QUrl previews = urls[QLatin1String("previews")].toUrl();
            d->imageUrls.insert(ImageTiny, tiny);
            d->imageUrls.insert(ImageSmall, small);
            d->imageUrls.insert(ImageMedium, medium);
            d->imageUrls.insert(ImageBig, big);
            d->imageUrls.insert(ImageLarge, large);
            d->imageUrls.insert(ImageHuge, huge);
            d->imageUrls.insert(ImagePreviews, previews);
        }
        emit signedOn(this);
    }
}

QMap<ImageUrl, QUrl> SignOnJob::imageUrls() const
{
    return d->imageUrls;
}

#include "signonjob.moc"
