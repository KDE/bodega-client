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
#include <QFile>
#include <QFileInfo>

#include "assetoperations.h"
#include "bookhandler.h"

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
    QFileInfo fileInfo(localFile);
    QFile f(localFile);

    QDir dir(QDir::homePath() + QString::fromLatin1("/Books/"));

    if (!dir.exists()) {
        dir.mkdir(dir.path());
    }

    QString path = QString::fromLatin1("%1.%2")
            .arg(m_handler->operations()->assetInfo().name)
            .arg(fileInfo.completeSuffix());
    f.rename(dir.filePath(path));
    setFinished();
}

}

#include "bookinstalljob.moc"
