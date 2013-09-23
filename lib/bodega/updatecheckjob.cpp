/*
 *   Copyright 2013 Coherent Theory LLC
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

#include "bodega/updatecheckjob.h"

namespace Bodega {

class UpdateCheckJob::Private
{
public:
    QStringList assets;
};

UpdateCheckJob::UpdateCheckJob(QNetworkReply *reply, Session *parent)
    : NetworkJob(reply, parent, true),
      d(new Private)
{
}

UpdateCheckJob::~UpdateCheckJob()
{
    delete d;
}

QStringList UpdateCheckJob::updatedAssets() const
{
    return d->assets;
}

void UpdateCheckJob::netFinished(const QVariantMap &jsonMap)
{
    d->assets = jsonMap[QLatin1String("assets")].toStringList();
}

}

#include "updatecheckjob.moc"

