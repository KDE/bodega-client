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

#include "installjobscheduler.h"

#include "session.h"
#include "assetoperations.h"
#include "installjob.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

using namespace Bodega;

class InstallJobScheduler::Private
{
public:
    Private(InstallJobScheduler *parent);
    void fetchAssetOperations(const QString &assetId, Bodega::Session *session);
    void operationReady();
    void jobFinished(Bodega::NetworkJob *job);

    InstallJobScheduler *q;
    int maximumRunning;
    QSet<QString> completedAssets;
    QStringList waitingQueue;
    QStringList runningQueue;
    QHash<AssetOperations *, Session *> pendingOperations;
    QHash<QString, Session *> pendingSessionsForId;

    QHash<QString, InstallJob *> assetIdToJob;
    QHash<InstallJob *, QString> jobToAssetId;

    static InstallJobScheduler* s_self;
};

InstallJobScheduler* InstallJobScheduler::Private::s_self = 0;

InstallJobScheduler::Private::Private(InstallJobScheduler *parent)
    : q(parent),
      maximumRunning(3)
{
}


void InstallJobScheduler::Private::fetchAssetOperations(const QString &assetId, Bodega::Session *session)
{
    AssetOperations *operation = session->assetOperations(assetId);
    pendingOperations[operation] = session;
    runningQueue << assetId;
    waitingQueue.removeAll(assetId);
    emit q->installStatusChanged(assetId, InstallJobScheduler::Installing);
    connect(operation, SIGNAL(ready()), q, SLOT(operationReady()));
}

void InstallJobScheduler::Private::operationReady()
{
    AssetOperations *operation = qobject_cast<AssetOperations *>(q->sender());

    if (!operation) {
        return;
    }

    assetIdToJob[operation->assetInfo().id] = pendingOperations[operation]->install(operation);
    jobToAssetId[assetIdToJob[operation->assetInfo().id]] = operation->assetInfo().id;
    pendingOperations.remove(operation);

    connect(assetIdToJob[operation->assetInfo().id], SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(jobFinished(Bodega::NetworkJob *)));
}

void InstallJobScheduler::Private::jobFinished(Bodega::NetworkJob *job)
{
    QString assetId = jobToAssetId.value(qobject_cast<InstallJob *>(job));

    completedAssets.insert(assetId);
    emit q->installStatusChanged(assetId, InstallJobScheduler::Finished);

    runningQueue.removeAll(assetId);
    assetIdToJob.remove(assetId);
    jobToAssetId.remove(qobject_cast<InstallJob *>(job));

    
    while (!waitingQueue.isEmpty() && runningQueue.size() < maximumRunning) {
        assetId = waitingQueue.first();
        waitingQueue.pop_front();
        //can this ever be false?
        if (pendingSessionsForId.contains(assetId)) {
            fetchAssetOperations(assetId, pendingSessionsForId.value(assetId));
            pendingSessionsForId.remove(assetId);
        }
    }
}




InstallJobScheduler::InstallJobScheduler(QObject *parent)
    : QObject(parent),
      d(new Private(this))
{
}

InstallJobScheduler::~InstallJobScheduler()
{
    delete d;
}

InstallJobScheduler* InstallJobScheduler::self()
{
    if (!InstallJobScheduler::Private::s_self) {
        InstallJobScheduler::Private::s_self = new InstallJobScheduler;
    }

    return InstallJobScheduler::Private::s_self;
}

void InstallJobScheduler::scheduleInstall(const QString &assetId, Bodega::Session *session)
{
    d->waitingQueue << assetId;
    emit installStatusChanged(assetId, Waiting);

    if (d->runningQueue.length() < d->maximumRunning) {
        d->fetchAssetOperations(assetId, session);
    } else {
        d->pendingSessionsForId[assetId] = session;
    }
}

Bodega::InstallJobScheduler::InstallStatus InstallJobScheduler::installStatus(const QString &assetId)
{
    if (d->completedAssets.contains(assetId)) {
        return Finished;
    } else if (d->waitingQueue.contains(assetId)) {
        return Waiting;
    } else if (d->runningQueue.contains(assetId)) {
        return Installing;
    } else {
        return Idle;
    }
}

Bodega::InstallJob *InstallJobScheduler::installJobForAsset(const QString &assetId)
{
    return d->assetIdToJob.value(assetId);
}


#include "installjobscheduler.moc"
