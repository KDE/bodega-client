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

#include "uninstalljob.h"

#include "session.h"

#include <QDebug>
#include <QTemporaryFile>

using namespace Bodega;

class UninstallJob::Private {
public:
    Private(UninstallJob *uj);

    UninstallJob *q;
    bool finished;
    bool failed;
    Error error;
};

UninstallJob::Private::Private(UninstallJob *uj)
    : q(uj),
      finished(false),
      failed(false)
{
}

UninstallJob::UninstallJob(Session *parent)
    : QObject(parent),
      d(new UninstallJob::Private(this))
{
}

UninstallJob::~UninstallJob()
{
    delete d;
}

bool UninstallJob::failed() const
{
    return d->failed;
}


bool UninstallJob::isFinished() const
{
    return d->finished;
}

Session *UninstallJob::session() const
{
    return qobject_cast<Session*>(parent());
}

Bodega::Error UninstallJob::error() const
{
    return d->error;
}

#include "uninstalljob.moc"
