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
#include "wallpaperhandler.h"
#include "session.h"

#include <QDebug>
#include <QFile>
#include <QDir>

#include <KStandardDirs>
#include <Plasma/Package>
#include <Plasma/Wallpaper>

using namespace Bodega;

WallpaperInstallJob::WallpaperInstallJob(QNetworkReply *reply, Session *session,
                               WallpaperHandler *handler)
    : InstallJob(reply, session),
      m_handler(handler)
{
}

WallpaperInstallJob::~WallpaperInstallJob()
{
}

void WallpaperInstallJob::downloadFinished(const QString &localFile)
{
    const QString root = KStandardDirs::locateLocal("wallpaper", QString());

    QString fileName = m_handler->operations()->assetInfo().filename;

    if (fileName.endsWith(QLatin1String(".wallpaper"))) {
        Plasma::PackageStructure package;
        package.setPath(localFile);
        const QString serviceType = package.metadata().serviceType();
        bool ret=false;
        if (package.metadata().serviceType() == QLatin1String("Plasma/Wallpaper")) {
            Plasma::PackageStructure::Ptr installer = Plasma::Wallpaper::packageStructure();
            ret = installer->installPackage(localFile, root);
        } else {
            Plasma::PackageStructure installer;
            ret = installer.installPackage(localFile, root);
        }

        if(!ret) {
            qWarning() << "couldn't install the wallpaper" << localFile;
        }

    } else if (fileName.toLower().endsWith(QLatin1String(".jpg")) ||
        fileName.toLower().endsWith(QLatin1String(".jpeg")) ||
        fileName.toLower().endsWith(QLatin1String(".png"))) {

        QFile f(localFile);

        QDir dir(root);

        if (!dir.exists()) {
            dir.mkdir(dir.path());
        }

        if (!f.rename(dir.filePath(fileName))) {
            setError(Error(Error::Session,
                        QLatin1String("ij/01"),
                        tr("Install failed"),
                        tr("Impossible to install the wallpaper, wrong permissions or no space left on device.")));
        }
    } else {
        setError(Error(Error::Session,
                        QLatin1String("ij/01"),
                        tr("Install failed"),
                        tr("Impossible to install the wallpaper, unknown file type.")));
    }

    setFinished();
}

#include "wallpaperinstalljob.moc"
