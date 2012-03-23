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

namespace Bodega
{

class AssetOperations::Private {
public:
    Private(AssetOperations *operations)
        : q(operations),
          handler(0)
    {}

    ~Private()
    {}

    void assetDownloadComplete(NetworkJob *job);
    bool ready();

    AssetOperations *q;
    AssetHandler *handler;
    AssetInfo assetInfo;
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
        emit q->ready();
    } else {
        emit q->failed();
    }
}

bool AssetOperations::Private::ready()
{
    return handler;
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
        return d->handler->install(reply, session);
    }

    return new InstallJob(reply, session);
}

Bodega::UninstallJob *AssetOperations::uninstall()
{
    if (d->ready()) {
        return d->handler->uninstall();
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
