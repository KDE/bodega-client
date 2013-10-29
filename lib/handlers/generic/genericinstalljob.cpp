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


#include "genericinstalljob.h"
#include "generichandler.h"
#include "filethread.h"

#include <QDebug>
#include <QThreadPool>

using namespace Bodega;

GenericInstallJob::GenericInstallJob(QNetworkReply *reply, Session *session, GenericHandler *handler)
    : InstallJob(reply, session),
      m_handler(handler)
{
}

GenericInstallJob::~GenericInstallJob()
{
}

void GenericInstallJob::downloadFinished(const QString &file)
{
    FileThread *thread = new FileThread(file, m_handler->installPath());
    connect(thread, SIGNAL(completed(Bodega::Error)), this, SLOT(threadFinished(Bodega::Error)));
    QThreadPool::globalInstance()->start(thread);
}

void GenericInstallJob::threadFinished(const Bodega::Error &error)
{
    if (error.type() != Bodega::Error::NoError) {
        setError(error);
    }

    setFinished();
}

#include "genericinstalljob.moc"
