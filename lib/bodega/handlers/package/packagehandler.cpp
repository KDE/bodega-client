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
#include <assethandlerfactory.h>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtGui/QDesktopServices>

#include <KService>
#include <KServiceTypeTrader>
#include <KSycoca>
#include <KStandardDirs>
#include <Plasma/Package>

using namespace Bodega;

PackageHandler::PackageHandler(QObject *parent)
    : AssetHandler(parent),
      m_cachedInstalled(false)
{
    m_supportedTypes << QLatin1String("Plasma/Applet")
                     << QLatin1String("Plasma/PopupApplet")
                     << QLatin1String("Plasma/Containment")
                     << QLatin1String("Plasma/DataEngine")
                     << QLatin1String("Plasma/Runner")
                     << QLatin1String("Plasma/Wallpaper")
                     << QLatin1String("Plasma/LayoutTemplate")
                     << QLatin1String("KWin/Effect")
                     << QLatin1String("KWin/WindowSwitcher")
                     << QLatin1String("KWin/Script");
    connect(KSycoca::self(), SIGNAL(databaseChanged(QStringList)), this, SLOT(checkInstalledChanged()));
    setReady(true);
}

PackageHandler::~PackageHandler()
{
}

void PackageHandler::init()
{
    m_cachedInstalled = isInstalled();
}

Plasma::PackageStructure *PackageHandler::createPackageStructure() const
{
    if (!operations()->assetTags().contains(QLatin1String("servicetype"))) {
        return 0;
    }

    QString serviceType = operations()->assetTags().value(QLatin1String("servicetype"));
    QString servicePrefix;
    Plasma::PackageStructure *installer = 0;

    if (serviceType.contains(QLatin1String("Plasma/Applet")) ||
        serviceType.contains(QLatin1String("Plasma/PopupApplet")) ||
        serviceType.contains(QLatin1String("Plasma/Containment"))) {
        servicePrefix = QLatin1String("plasma-applet-");

    //FIXME: themes are still broken
    /*} else if (type == QLatin1String("theme")) {
        packageRoot = QLatin1String("desktoptheme/");*/

    } else if (serviceType == QLatin1String("Plasma/DataEngine")) {
        servicePrefix = QLatin1String("plasma-dataengine-");

    } else if (serviceType == QLatin1String("Plasma/Runner")) {
        servicePrefix = QLatin1String("plasma-runner-");

    } else if (serviceType == QLatin1String("Plasma/Wallpaper")) {
        servicePrefix = QLatin1String("plasma-wallpaper-");

    } else if (serviceType == QLatin1String("Plasma/LayoutTemplate")) {
        servicePrefix = QLatin1String("plasma-layout-");

    } else if (serviceType == QLatin1String("KWin/Effect")) {
        servicePrefix = QLatin1String("kwin-effect-");

    } else if (serviceType == QLatin1String("KWin/WindowSwitcher")) {
        servicePrefix = QLatin1String("kwin-windowswitcher-");

    } else if (serviceType == QLatin1String("KWin/Script")) {
        servicePrefix = QLatin1String("kwin-script-");

    } else {
        const QString constraint = QString(QLatin1String("[X-KDE-ServiceType] == '%1'")).arg(serviceType);
        KService::List offers = KServiceTypeTrader::self()->query(QLatin1String("Plasma/PackageStructure"), constraint);
        if (offers.isEmpty()) {
            return 0;
        }

        KService::Ptr offer = offers.first();
        QString error;
        installer = offer->createInstance<Plasma::PackageStructure>(0, QVariantList(), &error);

        if (!installer) {
            return 0;
        }

    }

    // install, remove or upgrade
    if (!installer) {
        installer = new Plasma::PackageStructure(0, serviceType);
        installer->setServicePrefix(servicePrefix);
    }

    return installer;
}

bool PackageHandler::isInstalled() const
{
    if (!operations()->assetTags().contains(QLatin1String("pluginname"))) {
        return false;
    }
    const QString packageName = operations()->assetTags().value(QLatin1String("pluginname"));

    foreach (const QString& type, m_supportedTypes) {
        const KService::List services = KServiceTypeTrader::self()->query(type);
        foreach (const KService::Ptr &service, services) {
            if (packageName == service->property(QLatin1String("X-KDE-PluginInfo-Name"), QVariant::String).toString()) {
                return true;
            }
        }
    }

    return false;
}

void PackageHandler::checkInstalledChanged()
{
    if (m_cachedInstalled != isInstalled()) {
        m_cachedInstalled = isInstalled();
        emit installedChanged();
    }
}

Bodega::InstallJob *PackageHandler::install(QNetworkReply *reply, Session *session)
{
    if (!m_installJob) {
        m_installJob = new PackageInstallJob(reply, session, this);
        connect(m_installJob.data(), SIGNAL(jobFinished(Bodega::NetworkJob*)),
                this, SLOT(registerForUpdates(Bodega::NetworkJob*)));
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

class PackageHandlerFactory : public Bodega::AssetHandlerFactory
{
public:
    virtual AssetHandler* createHandler() { return new PackageHandler; }
};

#include "packagehandler.moc"
Q_EXPORT_PLUGIN2(packagehandler, PackageHandlerFactory);
