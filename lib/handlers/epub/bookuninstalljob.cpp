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

#include "bookuninstalljob.h"

#include <QDebug>
#include <QThreadPool>

#include "bookhandler.h"
#include "filethread.h"

using namespace Bodega;

BookUninstallJob::BookUninstallJob(Session *parent, BookHandler *handler)
    : UninstallJob(parent),
      m_handler(handler)
{
    QMetaObject::invokeMethod(this, "start", Qt::QueuedConnection);
}

void BookUninstallJob::start()
{
    FileThread *thread = new FileThread(m_handler->filePath());
    connect(thread, SIGNAL(completed(Bodega::Error)), this, SLOT(threadCompleted(Bodega::Error)));
    QThreadPool::globalInstance()->start(thread);
}

BookUninstallJob::~BookUninstallJob()
{
}

void BookUninstallJob::threadCompleted(const Bodega::Error &error)
{
    if (error.type() != Bodega::Error::NoError) {
        setError(error);
    }

    setFinished();
}


#include "bookuninstalljob.moc"
