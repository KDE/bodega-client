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

#include "rpmuninstalljob.h"
#include "rpmhandler.h"
#include "session.h"

#include <QDebug>
#include <QFile>

using namespace Bodega;

RpmUninstallJob::RpmUninstallJob(Session *parent, RpmHandler *handler)
    : UninstallJob(parent)
{
    if (1) {
        setError(Error(Error::Parsing,
                       QLatin1String("uj/01"),
                       tr("Uninstall failed"),
                       tr("Impossible to delete the file.")));
    }
    setFinished();
}

RpmUninstallJob::~RpmUninstallJob()
{
}

#include "rpmuninstalljob.moc"
