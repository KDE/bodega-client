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


#include "packageinstalljob.h"
#include "session.h"

#include <QDBusInterface>
#include <QDebug>
#include <QFile>

#include <KGlobal>
#include <KService>
#include <KServiceTypeTrader>
#include <KStandardDirs>
#include <Plasma/Package>

using namespace Bodega;

PackageInstallJob::PackageInstallJob(QNetworkReply *reply, Session *session)
    : InstallJob(reply, session)
{
}

PackageInstallJob::~PackageInstallJob()
{
}

void PackageInstallJob::downloadFinished(const QString &packageFile)
{
    //Figure out the type of package
    QString type;
    QString packageRoot;
    QString servicePrefix;
    Plasma::PackageStructure *installer = 0;
    // Check type for common plasma packages
    Plasma::PackageStructure package;
    package.setPath(packageFile);
    QString serviceType = package.metadata().serviceType();
    if (!serviceType.isEmpty()) {
        if (serviceType.contains(QLatin1String("Plasma/Applet")) ||
            serviceType.contains(QLatin1String("Plasma/PopupApplet")) ||
            serviceType.contains(QLatin1String("Plasma/Containment"))) {
            type = QLatin1String("plasmoid");
        } else if (serviceType == QLatin1String("Plasma/DataEngine")) {
            type = QLatin1String("dataengine");
        } else if (serviceType == QLatin1String("Plasma/Runner")) {
            type = QLatin1String("runner");
        } else if (serviceType == QLatin1String("Plasma/Wallpaper")) {
            type = QLatin1String("wallpaperplugin");
        } else if (serviceType == QLatin1String("Plasma/LayoutTemplate")) {
            packageRoot = QLatin1String("plasma/layout-templates/");
        } else if (serviceType == QLatin1String("KWin/Effect")) {
            packageRoot = QLatin1String("kwin/effects/");
        } else if (serviceType == QLatin1String("KWin/WindowSwitcher")) {
            packageRoot = QLatin1String("kwin/tabbox/");
        } else if (serviceType == QLatin1String("KWin/Script")) {
            packageRoot = QLatin1String("kwin/scripts/");
        } else {
            type = serviceType;
            qDebug() << "fallthrough type is" << serviceType;
        }
    }



    if (type == QLatin1String("plasmoid")) {
        packageRoot = QLatin1String("plasma/plasmoids/");
        servicePrefix = QLatin1String("plasma-applet-");
    } else if (type == QLatin1String("theme")) {
        packageRoot = QLatin1String("desktoptheme/");
    } else if (type == QLatin1String("wallpaper")) {
        packageRoot = QLatin1String("wallpapers/");
    } else if (type == QLatin1String("dataengine")) {
        packageRoot = QLatin1String("plasma/dataengines/");
        servicePrefix = QLatin1String("plasma-dataengine-");
    } else if (type == QLatin1String("runner")) {
        packageRoot = QLatin1String("plasma/runners/");
        servicePrefix = QLatin1String("plasma-runner-");
    } else if (type == QLatin1String("wallpaperplugin")) {
        packageRoot = QLatin1String("plasma/wallpapers/");
        servicePrefix = QLatin1String("plasma-wallpaper-");
    } else if (type == QLatin1String("layout-template")) {
        packageRoot = QLatin1String("plasma/layout-templates/");
        servicePrefix = QLatin1String("plasma-layout-");
    } else if (type == QLatin1String("kwineffect")) {
        packageRoot = QLatin1String("kwin/effects/");
        servicePrefix = QLatin1String("kwin-effect-");
    } else if (type == QLatin1String("windowswitcher")) {
        packageRoot = QLatin1String("kwin/tabbox/");
        servicePrefix = QLatin1String("kwin-windowswitcher-");
    } else if (type == QLatin1String("kwinscript")) {
        packageRoot = QLatin1String("kwin/scripts/");
        servicePrefix = QLatin1String("kwin-script-");
    } else {
        const QString constraint = QString(QLatin1String("[X-KDE-PluginInfo-Name] == '%1'")).arg(type);
        KService::List offers = KServiceTypeTrader::self()->query(QLatin1String("Plasma/PackageStructure"), constraint);
        if (offers.isEmpty()) {
            setError(Error(Error::Session,
                       QLatin1String("package/01"),
                       tr("Install failed"),
                       tr(QString::fromLatin1("Could not find a suitable installer for package of type %1").arg(type).toLatin1())));
            return;
        }

        KService::Ptr offer = offers.first();
        QString error;
        installer = offer->createInstance<Plasma::PackageStructure>(0, QVariantList(), &error);

        if (!installer) {
            setError(Error(Error::Session,
                       QLatin1String("package/02"),
                       tr("Install failed"),
                       tr(QString::fromLatin1("Could not load installer for package of type %1. Error reported was: %2").arg(type).arg(error).toLatin1())));
            return;
        }

        packageRoot = installer->defaultPackageRoot();
    }

    // install, remove or upgrade
    if (!installer) {
        installer = new Plasma::PackageStructure();
        installer->setServicePrefix(servicePrefix);
    }

    packageRoot = KStandardDirs::locateLocal("data", packageRoot);


    if (installer->installPackage(packageFile, packageRoot)) {
        qDebug() << "Successfully installed" << packageFile;
    } else {
        setError(Error(Error::Session,
                       QLatin1String("package/02"),
                       tr("Install failed"),
                       tr(QString::fromLatin1("Installation of %1 failed.").arg(packageFile).toLatin1())));
        delete installer;
        return;
    }

    QDBusInterface dbus(QLatin1String("org.kde.kded"), QLatin1String("/kbuildsycoca"), QLatin1String("org.kde.kbuildsycoca"));
    dbus.call(QDBus::NoBlock, QLatin1String("recreate"));

    //Delete the downloaded package
    QFile f(packageFile);
    f.remove();
    setFinished();
}

#include "packageinstalljob.moc"
