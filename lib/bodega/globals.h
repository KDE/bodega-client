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
        QString licenseText;
        QString partnerId;
        QString partnerName;
        QString name;
        QString filename;
        QString version;
        QString description;
        QString mimetype;
        QMap<ImageUrl, QUrl> images;
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
            points = 0;
            canDownload = false;
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

    struct BallotInfo
    {
    public:
        enum BallotFlag {
            None     = 0,
            Public   = 1 << 0,
            Wishlist = 1 << 1
        };
        Q_DECLARE_FLAGS(BallotFlags, BallotFlag);

        QString id;
        QString name;
        BallotFlags flags;
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

    typedef QMultiHash<QString, QString> Tags;
}

Q_DECLARE_METATYPE(Bodega::AssetInfo)
Q_DECLARE_METATYPE(Bodega::BallotInfo)
Q_DECLARE_METATYPE(Bodega::ChannelInfo)
Q_DECLARE_METATYPE(Bodega::ChangeLog)
Q_DECLARE_METATYPE(QList<Bodega::BallotInfo>)
Q_DECLARE_METATYPE(QList<Bodega::AssetInfo>)
Q_DECLARE_METATYPE(Bodega::Tags)
Q_DECLARE_METATYPE(Bodega::ParticipantInfo)

#endif
