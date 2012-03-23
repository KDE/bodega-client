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


#include "wallpaperinstalljob.h"
#include "session.h"

#include <QDebug>
#include <QFile>

#include <KGlobal>
#include <KStandardDirs>
#include <Plasma/Package>

using namespace Bodega;

WallpaperInstallJob::WallpaperInstallJob(QNetworkReply *reply, Session *session)
    : InstallJob(reply, session)
{
}

WallpaperInstallJob::~WallpaperInstallJob()
{
}

void WallpaperInstallJob::downloadFinished(const QString &localFile)
{
    Plasma::PackageStructure installer(0, QLatin1String("Plasma/Wallpaper"));
    installer.installPackage(localFile, KGlobal::dirs()->findDirs("wallpaper", QLatin1String("")).first());

    //Delete the downloaded package
    QFile f(localFile);
    f.remove();
    setFinished();
}

#include "wallpaperinstalljob.moc"
