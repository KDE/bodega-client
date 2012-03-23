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

#include "imagelocator.h"

#include "networkjob.h"

namespace Bodega
{

class ImageLocator::Private
{
public:
    void init(ImageLocator *);
    QHash<ImageSize, QUrl> paths;
    QHash<ImageSize, QString> strings;
};

void ImageLocator::Private::init(ImageLocator *)
{
    paths.insert(Tiny, QUrl());
    paths.insert(Small, QUrl());
    paths.insert(Medium, QUrl());
    paths.insert(Large, QUrl());
    paths.insert(Huge, QUrl());

    strings.insert(Tiny, QString::fromLatin1("tiny"));
    strings.insert(Small, QString::fromLatin1("small"));
    strings.insert(Medium, QString::fromLatin1("medium"));
    strings.insert(Large, QString::fromLatin1("large"));
    strings.insert(Huge, QString::fromLatin1("huge"));
}

ImageLocator::ImageLocator(QObject *parent)
    : QObject(parent),
      d(new Private)
{
    d->init(this);
}

ImageLocator::~ImageLocator()
{
    delete d;
}

QUrl ImageLocator::imageUrl(const QString &relativePath, ImageSize size)
{
    QUrl url(d->paths.value(size));
    url.setPath(url.path() + relativePath);
    return url;
}

void ImageLocator::updateBaseUrls(Bodega::NetworkJob *job)
{
    if (!job->failed()) {
        QVariant v = job->parsedJson().value(QString::fromLatin1("imageUrls"));
        if (v.type() == QVariant::Map) {
            QVariantMap m = v.toMap();
            if (m.contains(d->strings[Tiny])) {
                d->paths.insert(Tiny, m.value(d->strings.value(Tiny)).toUrl());
            }

            if (m.contains(d->strings[Small])) {
                d->paths.insert(Small, QUrl());
            }

            if (m.contains(d->strings[Medium])) {
                d->paths.insert(Medium, QUrl());
            }

            if (m.contains(d->strings[Large])) {
                d->paths.insert(Large, QUrl());
            }

            if (m.contains(d->strings[Huge])) {
                d->paths.insert(Huge, QUrl());
            }
        }
    }
}

} // namespace Bodega

#include <imagelocator.moc>

