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


#include "assetoperations.h"

#include <QDebug>

#include "assethandler.h"
#include "installjob.h"
#include "uninstalljob.h"

namespace Bodega
{

class AssetOperations::Private {
public:
    Private(AssetOperations *operations)
        : q(operations),
          handler(0),
          wasInstalled(false)
    {}

    ~Private()
    {}

    void assetDownloadComplete(NetworkJob *job);
    bool ready();
    void checkInstalled();

    AssetOperations *q;
    AssetHandler *handler;
    AssetInfo assetInfo;
    bool wasInstalled;
};

void AssetOperations::Private::assetDownloadComplete(NetworkJob *job)
{
    AssetJob *assetJob = qobject_cast<AssetJob *>(job);
    Q_ASSERT(assetJob);

    if (!job->failed()) {
        assetInfo = assetJob->info();

        delete handler;
        handler = 0;

        //FIXME: may ever have more than one mimetype?

        QHash<QString, QString> tags = assetJob->tags();
        QHashIterator<QString, QString> it(tags);
        while (it.hasNext()) {
            it.next();
            if (it.key() == QLatin1String("mimetype")) {
                handler = AssetHandler::create(it.value(), q);
                break;
            }
        }
    }

    if (ready()) {
        QObject::connect(handler, SIGNAL(ready()), q, SIGNAL(ready()));
        if (handler->isReady()) {
            emit q->ready();
        }
    } else {
        emit q->failed();
    }
    checkInstalled();
}

bool AssetOperations::Private::ready()
{
    return handler && handler->isReady();
}

void AssetOperations::Private::checkInstalled()
{
    if (wasInstalled != handler->isInstalled()) {
        wasInstalled = handler->isInstalled();
        emit q->installedChanged();
    }
}

AssetOperations::AssetOperations(const QString &assetId, Session *session)
    : QObject(session),
      d(new AssetOperations::Private(this))
{
    AssetJob *aj = session->asset(assetId);
    QObject::connect(aj, SIGNAL(jobFinished(Bodega::NetworkJob*)),
                     this, SLOT(assetDownloadComplete(Bodega::NetworkJob*)));
}

AssetOperations::~AssetOperations()
{
}

const AssetInfo &AssetOperations::assetInfo() const
{
    return d->assetInfo;
}

bool AssetOperations::isReady() const
{
    return d->ready();
}

QString AssetOperations::launchText() const
{
    if (d->ready()) {
        return d->handler->launchText();
    }

    return QString();
}

bool AssetOperations::isInstalled() const
{
    if (d->ready()) {
        return d->handler->isInstalled();
    }

    return false;
}

Bodega::InstallJob *AssetOperations::install(QNetworkReply *reply, Session *session)
{
    if (d->ready()) {
        Bodega::InstallJob *job = d->handler->install(reply, session);
        if (job) {
            connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)), this, SLOT(checkInstalled()));
        }
        return job;
    }

    return new InstallJob(reply, session);
}

Bodega::UninstallJob *AssetOperations::uninstall(Session *session)
{
    if (d->ready()) {
        Bodega::UninstallJob *job = d->handler->uninstall(session);
        if (job) {
            connect(job, SIGNAL(jobFinished(Bodega::UninstallJob *)), this, SLOT(checkInstalled()));
            d->checkInstalled();
        }
        return job;
    }

    return 0;
}

void AssetOperations::launch()
{
    if (d->ready()) {
        d->handler->launch();
    }
}

}

#include "assetoperations.moc"
