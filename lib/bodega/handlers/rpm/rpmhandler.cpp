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
}

RpmHandler::~RpmHandler()
{
}

void RpmHandler::init()
{
    PackageKit::Transaction *t = new PackageKit::Transaction;
    t->resolve(packageName());
    connect(t, SIGNAL(package(const PackageKit::Package &)),
            this, SLOT(gotPackage(const PackageKit::Package &)));
}

QString RpmHandler::packageName() const
{
    const QString name = operations()->assetInfo().path.path().replace(QRegExp(QLatin1String("(.*\\/)*([^-]*)-\\d.*")), QLatin1String("\\2"));
    qDebug() << "Asked for package name of" << operations()->assetInfo().path.path() << "Got" << name;
    //try to extract a package name from a file name
    return name;
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
        connect(m_installJob.data(), SIGNAL(jobFinished(Bodega::NetworkJob *)),
                this, SLOT(installJobFinished()));
    }

    return m_installJob.data();
}

Bodega::UninstallJob *RpmHandler::uninstall(Session *session)
{
    if (!m_uninstallJob) {
        m_uninstallJob = new RpmUninstallJob(session, this);
        connect(m_uninstallJob.data(), SIGNAL(jobFinished(Bodega::UninstallJob *)),
                this, SLOT(installJobFinished()));
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
    setReady(true);
    emit installedChanged();

    PackageKit::Transaction *listT = new PackageKit::Transaction;
    listT->getFiles(package);
    connect(listT, SIGNAL(files(const PackageKit::Package &, const QStringList &)),
            this, SLOT(gotFiles(const PackageKit::Package &, const QStringList &)));
}

void RpmHandler::installJobFinished()
{
    PackageKit::Transaction *t = new PackageKit::Transaction;
    t->resolve(packageName());
    connect(t, SIGNAL(package(const PackageKit::Package &)),
            this, SLOT(gotPackage(const PackageKit::Package &)));
}

void RpmHandler::gotFiles(const PackageKit::Package &package, const QStringList &fileNames)
{
    Q_UNUSED(package)
    qDebug() << fileNames;
    foreach (const QString &file, fileNames) {
        if (file.contains(QRegExp(QLatin1String(".*/applications/.*\\.desktop")))) {
            m_desktopFile = file;
            break;
        }
    }
}

#include "rpmhandler.moc"
Q_EXPORT_PLUGIN2(rpmhandler, Bodega::RpmHandler);

