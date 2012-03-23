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

#ifndef BODEGA_GLOBALS_H
#define BODEGA_GLOBALS_H

#include <QtCore/QUrl>
#include <QtCore/QDateTime>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/qglobal.h>

#ifndef BODEGA_EXPORT
# if defined(BODEGA_MAKEDLL)
   /* We are building this library */
#  define BODEGA_EXPORT Q_DECL_EXPORT
# else
   /* We are using this library */
#  define BODEGA_EXPORT Q_DECL_IMPORT
# endif
#endif

namespace Bodega {

    enum ImageUrl
    {
        ImageTiny = 1,
        ImageSmall,
        ImageMedium,
        ImageLarge,
        ImageHuge,
        ImagePreviews
    };

    struct ChannelInfo
    {
        QString id;
        QString name;
        QString description;
        int assetCount;
        QMap<ImageUrl, QUrl> images;
    };

    struct AssetInfo
    {
        QString id;
        QString license;
        QString partnerId;
        QString partnerName;
        QString name;
        QString version;
        QUrl    path;
        QMap<ImageUrl, QUrl> images;
        QString description;
        int     points;
        QString mimetype;

        AssetInfo()
            : points(0)
        {
        }

        void clear()
        {
            id = license = partnerId = partnerName = version = description = QString();
            path.clear();
            images.clear();
            points = 0;
        }
    };

    struct ChangeLog
    {
        struct Entry
        {
            QDateTime timestamp;
            QString changes;
        };
        // Version vs Change entries
        QMap<QString, Entry> entries;
    };

    typedef QMultiHash<QString, QString> Tags;
}

#endif
