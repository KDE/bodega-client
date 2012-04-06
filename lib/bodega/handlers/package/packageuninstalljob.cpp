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

#include <QDBusInterface>
#include <QDebug>
#include <QFile>

#include <KService>
#include <KServiceTypeTrader>
#include <KStandardDirs>

#include <Plasma/Package>

using namespace Bodega;

PackageUninstallJob::PackageUninstallJob(Session *parent, PackageHandler *handler)
    : UninstallJob(parent),
      m_handler(handler)
{
    if (!m_handler->operations()->assetTags().contains(QLatin1String("pluginname")) ||
        !m_handler->operations()->assetTags().contains(QLatin1String("servicetype"))) {
        setError(Error(Error::Session,
                       QLatin1String("packageuninstall/01"),
                       tr("Install failed"),
                       tr("Plugin name or service type tags not specified.")));
        setFinished();
        return;
    }

    const QString pluginName = m_handler->operations()->assetTags().value(QLatin1String("pluginname"));
    const QString serviceType = m_handler->operations()->assetTags().value(QLatin1String("servicetype"));

    Plasma::PackageStructure installer(0, serviceType);
    const QString packageRoot = KStandardDirs::locateLocal("data", installer.defaultPackageRoot());
    qDebug() << "Attempting to uninstall" << pluginName << "from" << packageRoot;
    const bool success = installer.uninstallPackage(pluginName, packageRoot);

    if (!success) {
        setError(Error(Error::Parsing,
                       QLatin1String("packageuninstall/02"),
                       tr("Uninstall failed"),
                       tr("Impossible to uninstall the package.")));
    }

    QDBusInterface dbus(QLatin1String("org.kde.kded"), QLatin1String("/kbuildsycoca"), QLatin1String("org.kde.kbuildsycoca"));
    dbus.call(QDBus::NoBlock, QLatin1String("recreate"));

    setFinished();
}

PackageUninstallJob::~PackageUninstallJob()
{
}

#include "packageuninstalljob.moc"
