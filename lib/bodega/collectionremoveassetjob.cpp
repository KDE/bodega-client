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

#include "ballotremoveassetjob.h"

using namespace Bodega;

class BallotRemoveAssetJob::Private
{
public:
    Private()
    {}

    void init(BallotRemoveAssetJob *q, const QUrl &url);
    void parseBallot(const QVariantMap &result);
    BallotRemoveAssetJob *q;
    BallotInfo ballot;
};

void BallotRemoveAssetJob::Private::init(BallotRemoveAssetJob *parent,
                                      const QUrl &url)
{
    q = parent;
}

void BallotRemoveAssetJob::Private::parseBallot(const QVariantMap &result)
{
    QVariantMap ballot = result[QLatin1String("ballot")].toMap();
    BallotInfo info;
    info.id = ballot[QLatin1String("id")].toString();
    info.name = ballot[QLatin1String("name")].toString();
    info.flags = BallotInfo::None;
    if (ballot[QLatin1String("public")].toBool()) {
        info.flags |= BallotInfo::Public;
    }
    if (ballot[QLatin1String("wishlist")].toBool()) {
        info.flags |= BallotInfo::Wishlist;
    }

    this->ballot = info;
}

BallotRemoveAssetJob::BallotRemoveAssetJob(QNetworkReply *reply,
                                 Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

BallotRemoveAssetJob::~BallotRemoveAssetJob()
{
    delete d;
}

BallotInfo BallotRemoveAssetJob::ballot() const
{
    return d->ballot;
}

void BallotRemoveAssetJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parseBallot(result);
    }
}

#include "ballotremoveassetjob.moc"
