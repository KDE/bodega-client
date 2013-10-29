/*
 *   Copyright 2013 Coherent Theory LLC
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


#include "generichandler.h"
#include "genericinstalljob.h"
#include <assethandlerfactory.h>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>

using namespace Bodega;

GenericHandler::GenericHandler(QObject *parent)
    : AssetHandler(parent)
{
    setReady(true);
}

GenericHandler::~GenericHandler()
{
}

void GenericHandler::init()
{
}

bool GenericHandler::isInstalled() const
{
    return QFile::exists(installPath());
}

Bodega::InstallJob *GenericHandler::install(QNetworkReply *reply, Session *session)
{
    if (!m_installJob) {
        m_installJob = new GenericInstallJob(reply, session, this);
        connect(m_installJob.data(), SIGNAL(jobFinished(Bodega::NetworkJob*)),
                this, SLOT(registerForUpdates(Bodega::NetworkJob*)));
    }

    return m_installJob.data();
}

Bodega::UninstallJob *GenericHandler::uninstall(Session *session)
{
    //qDebug()<<m_uninstallJob;
    if (!m_uninstallJob) {
        m_uninstallJob = new GenericUninstallJob(session, this);
        connect(m_uninstallJob.data(), SIGNAL(jobFinished(Bodega::NetworkJob*)),
                this, SLOT(unregisterForUpdates(Bodega::NetworkJob*)));
    }

    return m_uninstallJob.data();
}

QString GenericHandler::launchText() const
{
    return tr("Open");
}

void GenericHandler::launch()
{
    QProcess::startDetached(QLatin1String("xdg-open"), QStringList() << installPath());
}

QString GenericHandler::installPath() const
{
    return QDir::homePath() + QLatin1String("/Downloads/") +
           operations()->assetInfo().filename;
}

class GenericHandlerFactory : public Bodega::AssetHandlerFactory
{
public:
    virtual AssetHandler* createHandler() { return new GenericHandler; }
};

#include "generichandler.moc"
Q_EXPORT_PLUGIN2(generichandler, GenericHandlerFactory);
