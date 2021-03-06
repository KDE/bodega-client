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
#include "installjobsmodel.h"
#include "ratingsmodel_p.h"
#include "uninstalljob.h"

namespace Bodega
{

class AssetOperations::Private {
public:
    Private(AssetOperations *operations)
        : q(operations),
          handler(0),
          wasInstalled(false),
          progress(0),
          ratingsModel(new RatingsModel(q))
    {}

    ~Private()
    {
        delete ratingsModel;
    }

    void assetDownloadComplete(NetworkJob *job);
    bool ready();
    void checkInstalled();
    void progressHasChanged(qreal progress);

    AssetOperations *q;
    AssetHandler *handler;
    AssetInfo assetInfo;
    ChangeLog changeLog;
    Tags assetTags;
    QString mimetype;
    QString assetType;
    bool wasInstalled;
    qreal progress;
    RatingsModel *ratingsModel;
};

void AssetOperations::Private::assetDownloadComplete(NetworkJob *job)
{
    AssetJob *assetJob = qobject_cast<AssetJob *>(job);
    Q_ASSERT(assetJob);

    if (!job->failed()) {
        assetInfo = assetJob->info();
        assetTags = assetJob->tags();
        changeLog = assetJob->changeLog();

        ratingsModel->setAssetJob(assetJob);

        emit q->infoReady();

        delete handler;
        handler = 0;

        const QHash<QString, QString> tags = assetJob->tags();
        mimetype = tags.value(QLatin1String("mimetype"));
        assetType = tags.value(QLatin1String("assetType"));
        if (!assetType.isEmpty() || !mimetype.isEmpty()) {
            handler = AssetHandler::create(assetType, mimetype, q);
        }
    }

    if (handler) {
        QObject::connect(handler, SIGNAL(ready()), q, SIGNAL(ready()));
        QObject::connect(handler, SIGNAL(ready()), q, SLOT(checkInstalled()));
        if (handler->isReady()) {
            emit q->ready();
        }
        QObject::connect(handler, SIGNAL(installedChanged()), q, SIGNAL(installedChanged()));
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
    if (ready() && wasInstalled != handler->isInstalled()) {
        wasInstalled = handler->isInstalled();
        emit q->installedChanged();
    }
}

void AssetOperations::Private::progressHasChanged(qreal prog)
{
    progress = prog;
    emit q->progressChanged(progress);
}

AssetOperations::AssetOperations(const QString &assetId, Session *session)
    : QObject(session),
      d(new AssetOperations::Private(this))
{
    d->ratingsModel->setSession(session);

    AssetJob *aj = session->asset(assetId, AssetJob::ShowRatings | AssetJob::ShowPreviews | AssetJob::ShowChangeLog);
    QObject::connect(aj, SIGNAL(jobFinished(Bodega::NetworkJob*)),
                     this, SLOT(assetDownloadComplete(Bodega::NetworkJob*)));
}

AssetOperations::~AssetOperations()
{
}

const ChangeLog &AssetOperations::changeLog() const
{
    return d->changeLog;
}

const AssetInfo &AssetOperations::assetInfo() const
{
    return d->assetInfo;
}

const Bodega::Tags& AssetOperations::assetTags() const
{
    return d->assetTags;
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

QString AssetOperations::mimetype() const
{
    return d->mimetype;
}

QString AssetOperations::assetType() const
{
    return d->assetType;
}

qreal AssetOperations::progress() const
{
    return d->progress;
}

Bodega::InstallJob *AssetOperations::install(QNetworkReply *reply, Session *session)
{
    if (d->ready()) {
        Bodega::InstallJob *job = d->handler->install(reply, session);
        if (job) {
            connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)), this, SLOT(checkInstalled()));
        }
        session->installJobsModel()->addJob(d->assetInfo, job);
        connect(job, SIGNAL(progressChanged(qreal)), this, SLOT(progressHasChanged(qreal)));
        return job;
    }

    return new InstallJob(reply, session);
}

Bodega::UninstallJob *AssetOperations::uninstall(Session *session)
{
    if (d->ready()) {
        Bodega::UninstallJob *job = d->handler->uninstall(session);
        if (job) {
            connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)), this, SLOT(checkInstalled()));
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

QAbstractItemModel *AssetOperations::ratingsModel()
{
    return d->ratingsModel;
}

}

#include "assetoperations.moc"
