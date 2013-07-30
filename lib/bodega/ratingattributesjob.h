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

#ifndef BODEGA_RATINGATTRIBUTESJOB_H
#define BODEGA_RATINGATTRIBUTESJOB_H

#include <bodega/networkjob.h>
#include <bodega/globals.h>

namespace Bodega {

    class BODEGA_EXPORT RatingAttributesJob : public NetworkJob
    {
        Q_OBJECT
        Q_PROPERTY(QList<Bodega::RatingAttributes> ratingAttributes READ ratingAttributes)
    public:
        RatingAttributesJob(QNetworkReply *reply,
                       Session *parent);
        ~RatingAttributesJob();

        QList<Bodega::RatingAttributes> ratingAttributes() const;

    protected:
        virtual void netFinished(const QVariantMap &result);

    private:
        class Private;
        Private * const d;
    };

}

#endif
