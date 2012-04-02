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

#include <bodega/participantinfojob.h>
#include <bodega/globals.h>

namespace Bodega {

ParticipantInfoJob::ParticipantInfoJob(QNetworkReply *reply, Session *parent)
    : NetworkJob(reply, parent),
      d(0)
{
}

ParticipantInfoJob::~ParticipantInfoJob()
{
    //delete d;
}

void ParticipantInfoJob::netFinished(const QVariantMap &jsonMap)
{
    ParticipantInfo info;
    parseCommon(jsonMap);

    if (authSuccess() && !failed()) {
        info.assetCount = jsonMap[QLatin1String("assetCount")].toInt();
        info.channelCount = jsonMap[QLatin1String("channelCount")].toInt();
        info.storeCount = jsonMap[QLatin1String("storeCount")].toInt();
        info.pointsOwed = jsonMap[QLatin1String("pointsOwed")].toInt();
        info.organization = jsonMap[QLatin1String("organization")].toString();
        info.firstName = jsonMap[QLatin1String("firstName")].toString();
        info.lastName = jsonMap[QLatin1String("lastName")].toString();
        info.email = jsonMap[QLatin1String("email")].toString();
        emit infoReceived(info);
    }
}

}

#include "participantinfojob.moc"

