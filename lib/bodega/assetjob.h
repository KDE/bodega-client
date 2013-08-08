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

#ifndef BODEGA_ASSETJOB_H
#define BODEGA_ASSETJOB_H

#include <bodega/globals.h>
#include <bodega/networkjob.h>

#include <QtCore/QStringList>

namespace Bodega {

    class BODEGA_EXPORT AssetJob : public NetworkJob
    {
        Q_OBJECT
        Q_PROPERTY(Bodega::AssetInfo info READ info)
        Q_PROPERTY(Bodega::Tags tags READ tags)

    public:
        enum AssetFlag {
            None          = 0,
            ShowChangeLog = 1 << 0,
            ShowPreviews  = 1 << 1,
            Ratings = 1 << 2
        };
        Q_DECLARE_FLAGS(AssetFlags, AssetFlag);
    public:
        AssetJob(const QString &assetId, QNetworkReply *reply,
                 Session *parent);
        ~AssetJob();

        AssetFlags flags() const;

        QString assetId() const;
        AssetInfo info() const;
        ChangeLog changeLog() const;
        QStringList previews() const;
        Tags tags() const;
        QString contentType() const;

    protected:
        virtual void netFinished(const QVariantMap &jsonMap);

    private:
        class Private;
        Private * const d;
    };
}


#endif
