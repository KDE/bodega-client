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


#include "rpmhandler.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtGui/QDesktopServices>

#include <Daemon>

#include "rpminstalljob.h"

using namespace Bodega;

RpmHandler::RpmHandler(QObject *parent)
    : AssetHandler(parent)
{
    PackageKit::Transaction *t = new PackageKit::Transaction;
    t->resolve(packageName());
    connect(t, SIGNAL(package(const PackageKit::Package &)),
            this, SLOT(gotPackage(const PackageKit::Package &)));
}

RpmHandler::~RpmHandler()
{
}

QString RpmHandler::packageName() const
{
    //try to extract a package name from a file name
    return operations()->assetInfo().path.path().replace(QRegExp(QLatin1String("(.*\\/)*(.*)-\\d.*")), QLatin1String("\\2"));
}

const PackageKit::Package &RpmHandler::package() const
{
    return m_package;
}

bool RpmHandler::isInstalled() const
{
    return !m_package.id().isEmpty();
}

Bodega::InstallJob *RpmHandler::install(QNetworkReply *reply, Session *session)
{
    if (!m_installJob) {
        m_installJob = new RpmInstallJob(reply, session, this);
    }

    return m_installJob.data();
}

Bodega::UninstallJob *RpmHandler::uninstall(Session *session)
{
    if (!m_uninstallJob) {
        m_uninstallJob = new RpmUninstallJob(session, this);
    }

    return m_uninstallJob.data();
}

QString RpmHandler::launchText() const
{
    return tr("Read rpm");
}

void RpmHandler::launch()
{
    //not supported yet
}

void RpmHandler::gotPackage(const PackageKit::Package &package)
{
    m_package = package;
    setReady(true);
    emit installedChanged();
}

void RpmHandler::installJobFinished()
{
    m_package = PackageKit::Package();
    emit installedChanged();
}

#include "rpmhandler.moc"
Q_EXPORT_PLUGIN2(epubhandler, Bodega::RpmHandler);

