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
#include "packageidinstalljob.h"

using namespace Bodega;

RpmHandler::RpmHandler(QObject *parent)
    : AssetHandler(parent)
{
}

RpmHandler::~RpmHandler()
{
}

void RpmHandler::init()
{
    QFileInfo info(operations()->assetInfo().filename);
    PackageKit::Transaction *t = new PackageKit::Transaction;
    t->resolve(packageName());

    connect(t, SIGNAL(package(const PackageKit::Package &)),
            this, SLOT(gotPackage(const PackageKit::Package &)));
    connect(t, SIGNAL(finished(PackageKit::Transaction::Exit, uint)),
            this, SLOT(resolveFinished(PackageKit::Transaction::Exit, uint)));
}

QString RpmHandler::remoteName() const
{
    const QString name = operations()->assetInfo().filename;
    qDebug() << "Asked for package name of" << operations()->assetInfo().filename  << "Got" << name;
    //try to extract a package name from a file name
    return name;
}

bool RpmHandler::remoteNameIsPackageId() const
{
    //Package ids are composed of 4 parts separed by mandattory ;
    //http://www.packagekit.org/gtk-doc/concepts.html#introduction-ideas-packageid
    return operations()->assetInfo().filename.split(QLatin1Char(';'), QString::KeepEmptyParts).count() == 4;
}

QString RpmHandler::packageName() const
{
    if (remoteNameIsPackageId()) {
        return remoteName().split(QLatin1Char(';'), QString::KeepEmptyParts).first();
    } else {
        return remoteName();
    }
}

const PackageKit::Package &RpmHandler::package() const
{
    return m_package;
}

bool RpmHandler::isInstalled() const
{
    //TODO: manage updates
    return !m_package.id().isEmpty() && (m_package.info() == PackageKit::Package::InfoInstalled ||
                m_package.info() == PackageKit::Package::InfoCollectionInstalled);
}

Bodega::InstallJob *RpmHandler::install(QNetworkReply *reply, Session *session)
{
    if (!m_installJob) {
        //Have a package id
        if (remoteNameIsPackageId()) {
            m_installJob = new PackageIdInstallJob(reply, session, this);
        } else {
            m_installJob = new RpmInstallJob(reply, session, this);
        }
        connect(m_installJob.data(), SIGNAL(jobFinished(Bodega::NetworkJob *)),
                this, SLOT(installJobFinished()));
    }

    return m_installJob.data();
}

Bodega::UninstallJob *RpmHandler::uninstall(Session *session)
{
    if (!m_uninstallJob) {
        m_uninstallJob = new RpmUninstallJob(session, this);
        //if not QueuedConnection resolve sudenly after the installation will fail
        connect(m_uninstallJob.data(), SIGNAL(jobFinished(Bodega::UninstallJob *)),
                this, SLOT(installJobFinished()), Qt::QueuedConnection);
    }

    return m_uninstallJob.data();
}

QString RpmHandler::launchText() const
{
    return tr("Launch");
}

void RpmHandler::launch()
{
    QDesktopServices::openUrl(QUrl(m_desktopFile));
}

void RpmHandler::gotPackage(const PackageKit::Package &package)
{
    m_package = package;

    emit installedChanged();

    PackageKit::Transaction *listT = new PackageKit::Transaction;
    listT->getFiles(package);
    connect(listT, SIGNAL(files(const PackageKit::Package &, const QStringList &)),
            this, SLOT(gotFiles(const PackageKit::Package &, const QStringList &)));
}

void RpmHandler::resolveFinished(PackageKit::Transaction::Exit status, uint runtime)
{
    Q_UNUSED(runtime)

    qDebug() << "Package resolving operation finished, got status:" << status;
    setReady(true);
}

void RpmHandler::installJobFinished()
{
    PackageKit::Transaction *t = new PackageKit::Transaction;
    m_package = PackageKit::Package();

    t->resolve(packageName());
    connect(t, SIGNAL(package(const PackageKit::Package &)),
            this, SLOT(gotPackage(const PackageKit::Package &)));
}

void RpmHandler::gotFiles(const PackageKit::Package &package, const QStringList &fileNames)
{
    Q_UNUSED(package)
    foreach (const QString &file, fileNames) {
        if (file.contains(QRegExp(QLatin1String(".*/applications/.*\\.desktop")))) {
            m_desktopFile = file;
            break;
        }
    }
}

#include "rpmhandler.moc"
Q_EXPORT_PLUGIN2(rpmhandler, Bodega::RpmHandler);

