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

namespace Bodega {

class ParticipantInfo::Private
{
public:
    Private()
        : assetCount(0),
          channelCount(0),
          storeCount(0),
          pointsOwed(0),
          succeeded(false)
    {
    }

    QString organization;
    int assetCount;
    int channelCount;
    int storeCount;
    int pointsOwed;
    bool succeeded;
};

ParticipantInfo::ParticipantInfo(const ParticipantInfo &info)
    : d(new Private(*info.d))
{
}

ParticipantInfo::ParticipantInfo()
    : d(new Private)
{
}

ParticipantInfo::~ParticipantInfo()
{
    delete d;
}

bool ParticipantInfo::succeeded() const
{
    return d->succeeded;
}

int ParticipantInfo::assetCount() const
{
    return d->assetCount;
}

int ParticipantInfo::channelCount() const
{
    return d->channelCount;
}

int ParticipantInfo::storeCount() const
{
    return d->storeCount;
}

int ParticipantInfo::pointsOwed() const
{
    return d->pointsOwed;
}

QString ParticipantInfo::organization() const
{
    return d->organization;
}

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
        info.d->assetCount = jsonMap[QLatin1String("assetCount")].toInt();
        info.d->channelCount = jsonMap[QLatin1String("channelCount")].toInt();
        info.d->storeCount = jsonMap[QLatin1String("storeCount")].toInt();
        info.d->pointsOwed = jsonMap[QLatin1String("pointsOwed")].toInt();
        info.d->organization = jsonMap[QLatin1String("organization")].toString();
        info.d->succeeded = true;
    }

    emit infoReceived(info);
}

}

#include "participantinfojob.moc"

