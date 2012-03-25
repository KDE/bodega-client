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


#include "bookhandler.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtGui/QDesktopServices>

#include "bookinstalljob.h"

using namespace Bodega;

BookHandler::BookHandler(QObject *parent)
    : AssetHandler(parent)
{
}

BookHandler::~BookHandler()
{
}

QString BookHandler::filePath() const
{
    QFileInfo remoteInfo(operations()->assetInfo().path.path());
    //Te id is to make sure to have an unique filename
    return QDir::homePath() +
           QLatin1String("/Books/") +
           operations()->assetInfo().name + QLatin1String(" - ") + operations()->assetInfo().id +
           QLatin1String(".") + remoteInfo.completeSuffix();
}

bool BookHandler::isInstalled() const
{
    QFile f(filePath());
    return f.exists();
}

Bodega::InstallJob *BookHandler::install(QNetworkReply *reply, Session *session)
{
    if (!m_installJob) {
        m_installJob = new BookInstallJob(reply, session, this);
    }

    return m_installJob.data();
}

Bodega::UninstallJob *BookHandler::uninstall()
{
    QFile f(filePath());
    f.remove();
    //FIXME: needs to be an actual job!
    return 0;
}

QString BookHandler::launchText() const
{
    return tr("Read book");
}

void BookHandler::launch()
{
    QDesktopServices::openUrl(QUrl(QLatin1String("file://") + filePath(), QUrl::TolerantMode));
}

#include "bookhandler.moc"
Q_EXPORT_PLUGIN2(epubhandler, Bodega::BookHandler);

