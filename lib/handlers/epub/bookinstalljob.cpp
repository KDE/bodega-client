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


#include "bookinstalljob.h"

#include <QDebug>
#include <QDir>
#include <QThreadPool>

#include "assetoperations.h"
#include "bookhandler.h"
#include "filethread.h"

namespace Bodega
{

BookInstallJob::BookInstallJob(QNetworkReply *reply, Session *session,
                               BookHandler *handler)
    : InstallJob(reply, session)
{
    m_handler = handler;
}

BookInstallJob::~BookInstallJob()
{
}

void BookInstallJob::downloadFinished(const QString &localFile)
{
    const QString author = m_handler->operations()->assetTags().value(QLatin1String("author"));
    FileThread *thread = new FileThread(localFile, m_handler->filePath(), author);
    connect(thread, SIGNAL(completed(Bodega::Error)), this, SLOT(threadCompleted(Bodega::Error)));
    QThreadPool::globalInstance()->start(thread);
}

void BookInstallJob::threadCompleted(const Bodega::Error &error)
{
    if (error.type() != Bodega::Error::NoError) {
        setError(error);
    }

    setFinished();
}

}

#include "bookinstalljob.moc"

