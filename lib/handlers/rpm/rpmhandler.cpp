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
#include <QProcess>

#include <Daemon>

#include "rpminstalljob.h"
#include "packageidinstalljob.h"
#include <assethandlerfactory.h>

using namespace Bodega;

RpmHandler::RpmHandler(QObject *parent)
    : AssetHandler(parent),
      m_transaction(new PackageKit::Transaction(this)),
      m_info(PackageKit::Transaction::InfoUnknown)
{
}

RpmHandler::~RpmHandler()
{
}

void RpmHandler::init()
{
    m_transaction->reset();
    m_transaction->resolve(packageName());

    connect(m_transaction, SIGNAL(package(PackageKit::Transaction::Info,QString,QString)),
            this, SLOT(gotPackage(PackageKit::Transaction::Info,QString,QString)));
    connect(m_transaction, SIGNAL(finished(PackageKit::Transaction::Exit, uint)),
            this, SLOT(resolveFinished(PackageKit::Transaction::Exit, uint)));
}

QString RpmHandler::remoteName() const
{
    const QString name = operations()->assetInfo().filename;
    qDebug() << "Asked for package name of" << operations()->assetInfo().filename  << "Got" << name;
    //try to extract a package name from a file name
    return name;
}

bool RpmHandler::remoteNameIsDescriptor() const
{
    //Package ids are composed of 4 parts separed by mandattory ;
    //http://www.packagekit.org/gtk-doc/concepts.html#introduction-ideas-packageid
    return operations()->assetInfo().filename.split(QLatin1Char('.'), QString::KeepEmptyParts).last() == QLatin1String("desc");
}

QString RpmHandler::packageName() const
{
    if (remoteNameIsDescriptor()) {
        return remoteName().split(QLatin1Char('.'), QString::KeepEmptyParts).first();
    //is a rpm name
    } else {
        return remoteName().split(QLatin1Char('-'), QString::KeepEmptyParts).first();
    }
}

QString RpmHandler::packageId() const
{
    return m_packageId;
}

bool RpmHandler::isInstalled() const
{
    //TODO: manage updates
    return !m_packageId.isEmpty() &&
           (m_info == PackageKit::Transaction::InfoInstalled ||
            m_info == PackageKit::Transaction::InfoCollectionInstalled);
}

Bodega::InstallJob *RpmHandler::install(QNetworkReply *reply, Session *session)
{
    if (!m_installJob) {
        //Have a package id
        if (remoteNameIsDescriptor()) {
            m_installJob = new PackageIdInstallJob(reply, session, this);
        } else {
            m_installJob = new RpmInstallJob(reply, session, this);
            connect(m_installJob.data(), SIGNAL(jobFinished(Bodega::NetworkJob*)),
                    this, SLOT(registerForUpdates(Bodega::NetworkJob*)));
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
        connect(m_uninstallJob.data(), SIGNAL(jobFinished(Bodega::NetworkJob*)),
                this, SLOT(installJobFinished()), Qt::QueuedConnection);
        connect(m_uninstallJob.data(), SIGNAL(jobFinished(Bodega::NetworkJob*)),
                this, SLOT(unregisterForUpdates(Bodega::NetworkJob*)));
    }

    return m_uninstallJob.data();
}

QString RpmHandler::launchText() const
{
    return tr("Launch");
}

void RpmHandler::launch()
{
    QProcess::startDetached(QString::fromLatin1("kioclient"), QStringList() << QString::fromLatin1("exec") << m_desktopFile);
}

void RpmHandler::gotPackage(PackageKit::Transaction::Info info, const QString &packageId, const QString &summary)
{
    Q_UNUSED(summary)

    m_packageId = packageId;
    m_info = info;

    emit installedChanged();

    m_transaction->reset();
    m_transaction->getFiles(m_packageId);
    connect(m_transaction, SIGNAL(files(QString,QStringList)),
            this, SLOT(gotFiles(QString,QStringList)));
}

void RpmHandler::resolveFinished(PackageKit::Transaction::Exit status, uint runtime)
{
    Q_UNUSED(runtime)

    qDebug() << "Package resolving operation finished, got status:" << status;
    setReady(true);
}

void RpmHandler::installJobFinished()
{
    m_packageId.clear();
    m_info = PackageKit::Transaction::InfoUnknown;

    m_transaction->reset();
    m_transaction->resolve(packageName());
    connect(m_transaction, SIGNAL(package(PackageKit::Transaction::Info,QString,QString)),
            this, SLOT(gotPackage(PackageKit::Transaction::Info,QString,QString)));
}

void RpmHandler::gotFiles(const QString &package, const QStringList &fileNames)
{
    Q_UNUSED(package)
    foreach (const QString &file, fileNames) {
        if (file.contains(QRegExp(QLatin1String(".*/applications/.*\\.desktop")))) {
            m_desktopFile = file;
            break;
        }
    }
}

class RpmHandlerFactory : public Bodega::AssetHandlerFactory
{
public:
    virtual AssetHandler* createHandler() { return new RpmHandler; }
};

#include "rpmhandler.moc"
Q_EXPORT_PLUGIN2(rpmhandler, RpmHandlerFactory);

