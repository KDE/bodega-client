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

#include "packageuninstalljob.h"
#include "packagehandler.h"
#include "session.h"

#include <QDebug>
#include <QFile>

#include <KService>
#include <KServiceTypeTrader>
#include <KStandardDirs>

#include <Plasma/Package>

using namespace Bodega;

PackageUninstallJob::PackageUninstallJob(Session *parent, PackageHandler *handler)
    : UninstallJob(parent)
{
    const QString packageName = handler->operations()->assetInfo().path.path().replace(QRegExp(QLatin1String(".*\\/([^\\/]*)\\..*")), QLatin1String("\\1"));

    QStringList pluginTypes;
    QString packageRoot;

    foreach (const QString& type, handler->supportedTypes()) {
        const KService::List services = KServiceTypeTrader::self()->query(type);
        foreach (const KService::Ptr &service, services) {
            if (packageName == service->property(QLatin1String("X-KDE-PluginInfo-Name"), QVariant::String).toString()) {
                pluginTypes = service->serviceTypes();
                //FIXME: desktop themes don't have servicetype
                if (!pluginTypes.isEmpty()) {
                    if (pluginTypes.contains(QLatin1String("Plasma/Applet")) ||
                        pluginTypes.contains(QLatin1String("Plasma/PopupApplet")) ||
                        pluginTypes.contains(QLatin1String("Plasma/Containment"))) {
                        packageRoot = QLatin1String("plasma/plasmoids/");
                    } else if (pluginTypes.contains(QLatin1String("Plasma/DataEngine"))) {
                        packageRoot = QLatin1String("plasma/dataengines/");
                    } else if (pluginTypes.contains(QLatin1String("Plasma/Runner"))) {
                        packageRoot = QLatin1String("plasma/runners/");
                    } else if (pluginTypes.contains(QLatin1String("Plasma/Wallpaper"))) {
                        packageRoot = QLatin1String("plasma/wallpapers/");
                    } else if (pluginTypes.contains(QLatin1String("Plasma/LayoutTemplate"))) {
                        packageRoot = QLatin1String("plasma/layout-templates/");
                    } else if (pluginTypes.contains(QLatin1String("KWin/Effect"))) {
                        packageRoot = QLatin1String("kwin/effects/");
                    } else if (pluginTypes.contains(QLatin1String("KWin/WindowSwitcher"))) {
                        packageRoot = QLatin1String("kwin/tabbox/");
                    } else if (pluginTypes.contains(QLatin1String("KWin/Script"))) {
                        packageRoot = QLatin1String("kwin/scripts/");
                    } else {
                        setError(Error(Error::Session,
                            QLatin1String("packageuninstall/01"),
                            tr("Uninstall failed"),
                            tr(QString::fromLatin1("Could not figure out the package type of %1").arg(packageName).toLatin1())));
                            setFinished();
                            return;
                    }
                }
                break;
            }
        }
    }

    Plasma::PackageStructure installer(0, pluginTypes.first());
    const bool success = installer.uninstallPackage(packageName, packageRoot);

    if (!success) {
        setError(Error(Error::Parsing,
                       QLatin1String("uj/01"),
                       tr("Uninstall failed"),
                       tr("Impossible to uninstall the package.")));
    }
    setFinished();
}

PackageUninstallJob::~PackageUninstallJob()
{
}

#include "packageuninstalljob.moc"
