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


#include "wallpaperhandler.h"
#include "wallpaperinstalljob.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtGui/QDesktopServices>

#include <KStandardDirs>
#include <Plasma/Package>

using namespace Bodega;

WallpaperHandler::WallpaperHandler(QObject *parent)
    : AssetHandler(parent)
{
}

WallpaperHandler::~WallpaperHandler()
{
}

bool WallpaperHandler::isInstalled() const
{
    QString packageName = operations()->assetInfo().path.path().replace(QRegExp(QLatin1String(".*\\/([^\\/]*)\\.wallpaper")), QLatin1String("\\1"));
    const QString path = KStandardDirs::locate("wallpaper", packageName + QLatin1String("/metadata.desktop"));
    return !path.isEmpty();
}

Bodega::InstallJob *WallpaperHandler::install(QNetworkReply *reply, Session *session)
{
    if (!m_installJob) {
        m_installJob = new WallpaperInstallJob(reply, session);
    }

    return m_installJob.data();
}

//TODO: make it a job
Bodega::UninstallJob *WallpaperHandler::uninstall(Session *session)
{qDebug()<<m_uninstallJob;
    if (!m_uninstallJob) {
        m_uninstallJob = new WallpaperUninstallJob(session, this);
    }

    return m_uninstallJob.data();
}

QString WallpaperHandler::launchText() const
{
    return QString();
}

void WallpaperHandler::launch()
{
    //This plugin doesn't support launch
}

#include "wallpaperhandler.moc"
Q_EXPORT_PLUGIN2(wallpaperhandler, Bodega::WallpaperHandler);
