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

#ifndef BODEGA_LISTBALLOTSJOB_H
#define BODEGA_LISTBALLOTSJOB_H

#include <bodega/networkjob.h>
#include <bodega/globals.h>

namespace Bodega {

    class BODEGA_EXPORT ListBallotsJob : public NetworkJob
    {
        Q_OBJECT
        Q_PROPERTY(int pageSize READ pageSize)
        Q_PROPERTY(int offset READ offset)
        Q_PROPERTY(bool hasMoreBallots READ hasMoreBallots)
        Q_PROPERTY(QList<Bodega::BallotInfo> ballots READ ballots)
    public:
        ListBallotsJob(QNetworkReply *reply,
                       Session *parent);
        ~ListBallotsJob();

        QList<Bodega::BallotInfo> ballots() const;

        bool hasMoreBallots() const;
        int offset() const;
        int pageSize() const;

    protected:
        virtual void netFinished(const QVariantMap &result);

    private:
        class Private;
        Private * const d;
    };

}

#endif
