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

#include "wallpaperuninstalljob.h"
#include "wallpaperhandler.h"
#include "session.h"

#include <QDebug>
#include <QFile>

#include <KStandardDirs>

#include <Plasma/Package>

using namespace Bodega;

WallpaperUninstallJob::WallpaperUninstallJob(Session *parent, WallpaperHandler *handler)
    : UninstallJob(parent)
    , m_handler(handler)
{
    QMetaObject::invokeMethod(this, "commit", Qt::QueuedConnection);
}

WallpaperUninstallJob::~WallpaperUninstallJob()
{
}

void WallpaperUninstallJob::commit()
{
    QString packageName = m_handler->assetLocation(false);

    if (packageName.endsWith(QLatin1String(".wallpaper"))) {
        Plasma::PackageStructure installer(0, QLatin1String("Plasma/Wallpaper"));
        const bool success = installer.uninstallPackage(packageName, KGlobal::dirs()->findDirs("wallpaper", QString()).first());

        if (!success) {
            setError(Error(Error::Parsing,
                        QLatin1String("uj/02"),
                        tr("Uninstall failed"),
                        tr("Impossible to uninstall the wallpaper package.")));
        }
    } else {
        QFile f(m_handler->assetLocation(true));
        if (!f.exists()) {
            setError(Error(Error::Session,
                        QLatin1String("uj/01"),
                        tr("Uninstall failed"),
                        tr("The wallpaper is not installed.")));
        } else if (!f.remove()) {
            setError(Error(Error::Session,
                        QLatin1String("uj/02"),
                        tr("Uninstall failed"),
                        tr("Impossible to delete the file.")));
        }
    }
    setFinished();
}

#include "wallpaperuninstalljob.moc"
