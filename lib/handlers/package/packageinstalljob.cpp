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
#include "packagehandler.h"
#include "session.h"

#include <QDBusInterface>
#include <QDebug>
#include <QFile>
#include <QFileInfo>

#include <KGlobal>
#include <KService>
#include <KServiceTypeTrader>
#include <KStandardDirs>
#include <Plasma/Package>

using namespace Bodega;

PackageInstallJob::PackageInstallJob(QNetworkReply *reply, Session *session, PackageHandler *handler)
    : InstallJob(reply, session),
      m_handler(handler)
{
}

PackageInstallJob::~PackageInstallJob()
{
}

void PackageInstallJob::downloadFinished(const QString &packageFile)
{
    QString packageRoot;
    QString servicePrefix;
    Plasma::PackageStructure *installer = m_handler->createPackageStructure();

    if (!installer) {
        setError(Error(Error::Session,
                       QLatin1String("packageinstall/04"),
                       tr("Install failed"),
                       tr(QString::fromLatin1("Could not create installer for %1.").arg(packageFile).toLatin1())));
        setFinished();
        return;
    }

    packageRoot = KStandardDirs::locateLocal("data", installer->defaultPackageRoot());

    QFileInfo info(packageFile);

    installer->setPath(packageFile);
    Plasma::PackageMetadata metadata = installer->metadata();

    //if it was already installed, remove it
    installer->uninstallPackage(metadata.pluginName(), packageRoot);

    if (installer->installPackage(packageFile, packageRoot)) {
        qDebug() << "Successfully installed" << packageFile;
        QDBusInterface dbus(QLatin1String("org.kde.kded"), QLatin1String("/kbuildsycoca"), QLatin1String("org.kde.kbuildsycoca"));
        dbus.call(QDBus::NoBlock, QLatin1String("recreate"));
    } else {
        setError(Error(Error::Session,
                       QLatin1String("packageinstall/04"),
                       tr("Install failed"),
                       tr(QString::fromLatin1("Installation of %1 failed.").arg(packageFile).toLatin1())));
    }

    delete installer;
    setFinished();
}

#include "packageinstalljob.moc"
