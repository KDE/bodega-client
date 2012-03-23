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

#ifndef BODEGA_PARTICIPANTINFOJOB_H
#define BODEGA_PARTICIPANTINFOJOB_H

#include <bodega/networkjob.h>
#include <bodega/globals.h>

namespace Bodega {

    class Session;

    class BODEGA_EXPORT ParticipantInfo
    {
    public:
        ParticipantInfo(const ParticipantInfo &info);
        ~ParticipantInfo();

        bool succeeded() const;
        int assetCount() const;
        int channelCount() const;
        int storeCount() const;
        int pointsOwed() const;
        QString organization() const;

    private:
        ParticipantInfo();

        friend class ParticipantInfoJob;
        class Private;
        Private *const d;
    };

    class BODEGA_EXPORT ParticipantInfoJob : public NetworkJob
    {
        Q_OBJECT
    public:
        ParticipantInfoJob(QNetworkReply *reply, Session *parent);
        ~ParticipantInfoJob();

    Q_SIGNALS:
        void infoReceived(const Bodega::ParticipantInfo &info);

    protected:
        virtual void netFinished(const QVariantMap &jsonMap);

    private:
        class Private;
        Private * const d;
    };
}

#endif
