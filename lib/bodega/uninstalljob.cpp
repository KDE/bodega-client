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
    void gotError(Bodega::UninstallJob *, const Bodega::Error &);
    void jobHasFinished(Bodega::UninstallJob *);

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
    connect(this, SIGNAL(error(Bodega::UninstallJob *, const Bodega::Error &)),
            this, SLOT(gotError(Bodega::UninstallJob *, const Bodega::Error &)));
    connect(this, SIGNAL(jobFinished(Bodega::UninstallJob *)),
            this, SLOT(jobHasFinished(Bodega::UninstallJob *)));
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

void UninstallJob::Private::gotError(Bodega::UninstallJob *job, const Bodega::Error &e)
{
    error = e;
    failed = true;
    emit q->failedChanged();
}

void UninstallJob::Private::jobHasFinished(Bodega::UninstallJob *job)
{
    finished = true;
    emit q->finishedChanged();
}

#include "uninstalljob.moc"
