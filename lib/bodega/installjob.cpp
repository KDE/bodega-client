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

#include "installjob.h"

#include "session.h"
#include "session_p.h"
#include "assetoperations.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

using namespace Bodega;

class InstallJob::Private
{
public:
    Private() {}
};

InstallJob::InstallJob(QNetworkReply *reply, Session *session)
    : NetworkJob(reply, session, false),
      d(0)
{
}

InstallJob::~InstallJob()
{
    delete d;
}

void InstallJob::downloadFinished(const QString &filename)
{
    setFinished();
}

#include "installjob.moc"
