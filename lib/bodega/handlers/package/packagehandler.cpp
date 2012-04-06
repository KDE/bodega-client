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


#include "packagehandler.h"
#include "packageinstalljob.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtGui/QDesktopServices>

#include <KService>
#include <KServiceTypeTrader>
#include <KStandardDirs>
#include <Plasma/Package>

using namespace Bodega;

PackageHandler::PackageHandler(QObject *parent)
    : AssetHandler(parent)
{
    setReady(true);
}

PackageHandler::~PackageHandler()
{
}

bool PackageHandler::isInstalled() const
{
    //FIXME: pluginName always == file name?
    const QString packageName = operations()->assetInfo().path.path().replace(QRegExp(QLatin1String(".*\\/([^\\/]*)\\..*")), QLatin1String("\\1"));

    QStringList types;
    types << QLatin1String("Plasma/Applet")
          << QLatin1String("Plasma/PopupApplet")
          << QLatin1String("Plasma/Containment")
          << QLatin1String("Plasma/DataEngine")
          << QLatin1String("Plasma/Runner")
          << QLatin1String("Plasma/Wallpaper")
          << QLatin1String("Plasma/LayoutTemplate")
          << QLatin1String("KWin/Effect")
          << QLatin1String("KWin/WindowSwitcher")
          << QLatin1String("KWin/Script");

    foreach (const QString& type, types) {
        const KService::List services = KServiceTypeTrader::self()->query(type);
        foreach (const KService::Ptr &service, services) {
            if (packageName == service->property(QLatin1String("X-KDE-PluginInfo-Name"), QVariant::String).toString()) {
                return true;
            }
        }
    }

    return false;
}

Bodega::InstallJob *PackageHandler::install(QNetworkReply *reply, Session *session)
{
    if (!m_installJob) {
        m_installJob = new PackageInstallJob(reply, session);
    }

    return m_installJob.data();
}

Bodega::UninstallJob *PackageHandler::uninstall(Session *session)
{
    //qDebug()<<m_uninstallJob;
    if (!m_uninstallJob) {
        m_uninstallJob = new PackageUninstallJob(session, this);
    }

    return m_uninstallJob.data();
}

QString PackageHandler::launchText() const
{
    return QString();
}

void PackageHandler::launch()
{
    //This plugin doesn't support launch
}

#include "packagehandler.moc"
Q_EXPORT_PLUGIN2(packagehandler, Bodega::PackageHandler);
