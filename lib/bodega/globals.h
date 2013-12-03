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
#include <QtCore/QMetaType>
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
        ImageBig,
        ImageLarge,
        ImageHuge,
        ImagePreviews
    };

    enum PreviewType {
        ScreenShot1 = 1,
        ScreenShot2,
        CoverFront = 100,
        CoverBack
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
        struct AssetInfoRatings
        {
            QString averageRating;
            QString ratingsCount;
            QString attributeId;
        };

        QList<AssetInfoRatings> ratings;
        QString id;
        QString license;
        QString licenseText;
        QString partnerId;
        QString partnerName;
        QString name;
        QString filename;
        QString version;
        QDateTime created;
        QString description;
        QString mimetype;
        QString formatedSize;
        QMap<ImageUrl, QUrl> images;
        QMap<PreviewType, QUrl> previews;
        qint64 size;
        int  points;
        bool canDownload;

        AssetInfo()
            : points(0),
              canDownload(false)
        {
        }

        void clear()
        {
            id = license = partnerId = partnerName = version = description = QString();
            images.clear();
            filename.clear();
            size = 0;
            points = 0;
            canDownload = false;
            ratings.clear();
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

    struct CollectionInfo
    {
    public:
        enum CollectionFlag {
            None     = 0,
            Public   = 1 << 0,
            Wishlist = 1 << 1
        };
        Q_DECLARE_FLAGS(CollectionFlags, CollectionFlag);

        QString id;
        QString name;
        CollectionFlags flags;
    };

    class BODEGA_EXPORT ParticipantInfo
    {
    public:
        int assetCount;
        int channelCount;
        int storeCount;
        int pointsOwed;
        QString organization;
        QString firstName;
        QString lastName;
        QString email;
    };

    struct RatingAttributes
    {
        QString id;
        QString name;
        QString lowDesc;
        QString highDesc;
        QString assetType;
    };

    struct Rating
    {
        QString attributeId;
        QString attributeName;
        QString rating;
    };

    struct AssetRatings
    {
        QString attributeId;
        QString attributeName;
        QString personId;
        QString personName;
        QDateTime rated;
        QList<Rating> ratings;
    };

    struct ParticipantRatings
    {
        QString assetId;
        QString assetDescription;
        QString assetName;
        QString assetVersion;
        QDateTime rated;
        QList<Rating> ratings;
    };

    typedef QMultiHash<QString, QString> Tags;
}

Q_DECLARE_METATYPE(Bodega::AssetInfo)
Q_DECLARE_METATYPE(Bodega::CollectionInfo)
Q_DECLARE_METATYPE(Bodega::ChannelInfo)
Q_DECLARE_METATYPE(Bodega::ChangeLog)
Q_DECLARE_METATYPE(Bodega::RatingAttributes)
Q_DECLARE_METATYPE(Bodega::AssetRatings)
Q_DECLARE_METATYPE(Bodega::Rating)
Q_DECLARE_METATYPE(QList<Bodega::CollectionInfo>)
Q_DECLARE_METATYPE(QList<Bodega::AssetInfo>)
Q_DECLARE_METATYPE(QList<Bodega::RatingAttributes>)
Q_DECLARE_METATYPE(QList<Bodega::Rating>)
Q_DECLARE_METATYPE(Bodega::Tags)
Q_DECLARE_METATYPE(Bodega::ParticipantInfo)

#endif
