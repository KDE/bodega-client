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

#include "bodega/updatescheckjob.h"

namespace Bodega {

class UpdatesCheckJob::Private
{
public:
    QStringList assets;
};

UpdatesCheckJob::UpdatesCheckJob(QNetworkReply *reply, Session *parent)
    : NetworkJob(reply, parent, true),
      d(new Private)
{
}

UpdatesCheckJob::~UpdatesCheckJob()
{
    delete d;
}

QStringList UpdatesCheckJob::updatedAssets() const
{
    return d->assets;
}

void UpdatesCheckJob::netFinished(const QVariantMap &jsonMap)
{
    d->assets = jsonMap[QLatin1String("assets")].toStringList();
}

}

#include "updatescheckjob.moc"

