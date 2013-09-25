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

#include "updatedassetsmodel.h"

#include "assethandler.h"
#include "assetbriefsjob.h"
#include "session.h"
#include "installjob.h"
#include "installjobsmodel.h"

#include <QEventLoop>
#include <QDebug>
#include <QFile>
#include <QMetaEnum>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#define SESSION_ID(store, warehouse) store + QLatin1Char('_') + warehouse

namespace Bodega {


class UpdatedAssetsModel::Private
{
public:
    Private(UpdatedAssetsModel *parent) :
        q(parent)
    {
    }

    ~Private()
    {
    }

    UpdatedAssetsModel *q;
    QList<AssetInfo> assets;
    QHash<QString, QStringList> pendingAssets;
    QHash<QString, Session *> sessions;
    QMap<int, Session*> sessionIndexes;
    QSqlDatabase db;
    QHash<QPersistentModelIndex, Bodega::InstallJob *> installJobs;
    QHash<Bodega::InstallJob *, QPersistentModelIndex> installJobsIndexes;

    void briefsJobFinished(Bodega::NetworkJob *);
    void fetchBriefs(const QString &store, const QString &warehouse, const QStringList &assets);
    void sessionAuthenticated(bool authed);
    void jobAdded(const Bodega::AssetInfo &info, Bodega::InstallJob *job);
    void progressChanged(qreal progress);
    void jobDestroyed(QObject *obj);

    static UpdatedAssetsModel *s_self;
};

UpdatedAssetsModel *UpdatedAssetsModel::Private::s_self = 0;

void UpdatedAssetsModel::Private::briefsJobFinished(Bodega::NetworkJob *job)
{
    AssetBriefsJob *briefsJob = qobject_cast<AssetBriefsJob *>(job);
    if (!briefsJob) {
        return;
    }

    if (briefsJob->assets().isEmpty()) {
        return;
    }

    q->beginInsertRows(QModelIndex(), assets.size(), assets.size() + briefsJob->assets().size());

    sessionIndexes.insert(assets.size(), briefsJob->session());
    assets.append(briefsJob->assets());
    q->endInsertRows();

    /*
    int count = 0;
    foreach (const AssetInfo &info, briefsJob->assets()) {
        QModelIndex index = q->createIndex(count++, 0);
        qDebug() << "Asset: " << info.id << info.name << info.partnerName << info.size
                 << q->data(index, UpdatedAssetsModel::SessionRole).value<Bodega::Session*>();
    }
    */
}

void UpdatedAssetsModel::Private::fetchBriefs(const QString &store, const QString &warehouse, const QStringList &assets)
{
    const QString sessionId(SESSION_ID(store, warehouse));
    Bodega::Session *session = sessions.value(sessionId);
    if (!session) {
        pendingAssets[sessionId].append(assets);
        session = new Bodega::Session(q);
        sessions.insert(sessionId, session);
        connect(session, SIGNAL(authenticated(bool)), q, SLOT(sessionAuthenticated(bool)));

        session->setBaseUrl(warehouse);
        session->setStoreId(store);
        //FIXME!
        session->setUserName(QLatin1String("zack@kde.org"));
        session->setPassword(QLatin1String("zack"));
        session->signOn();
    } else if (session->isAuthenticated()) {
        Bodega::AssetBriefsJob *job = session->assetBriefs(assets);
        connect(job, SIGNAL(jobFinished(Bodega::NetworkJob*)),
                q, SLOT(briefsJobFinished(Bodega::NetworkJob*)));
    } else {
        pendingAssets[sessionId].append(assets);
    }
}

void UpdatedAssetsModel::Private::sessionAuthenticated(bool authed)
{
    if (!authed) {
        //TODO: show an error, re-auth?
        return;
    }

    Bodega::Session *session = qobject_cast<Bodega::Session *>(q->sender());
    if (!session) {
        return;
    }

    disconnect(session, SIGNAL(authenticated(bool)), q, SLOT(sessionAuthenticated(bool)));

    const QString warehouse = session->baseUrl().toString();
    const QString sessionId = SESSION_ID(session->storeId(), warehouse);
    if (!pendingAssets.value(sessionId).isEmpty()) {
        fetchBriefs(session->storeId(), warehouse, pendingAssets.value(sessionId));
    }
}

void UpdatedAssetsModel::Private::jobAdded(const Bodega::AssetInfo &info, Bodega::InstallJob *job)
{
    connect(job, SIGNAL(progressChanged(qreal)), q, SLOT(progressChanged(qreal)));
    connect(job, SIGNAL(destroyed(QObject *)), q, SLOT(jobDestroyed(QObject *)));

    //FIXME: get rid of this linear scan
    int i = 0;
    foreach (const AssetInfo &existingInfo, assets) {
        if (existingInfo.id == info.id) {
            installJobs[QPersistentModelIndex(q->index(i, 0))] = job;
            installJobsIndexes[job] = QPersistentModelIndex(q->index(i, 0));
            emit(q->dataChanged(q->index(i, 0), q->index(i, 0)));
            break;
        }
        ++i;
    }
}

void UpdatedAssetsModel::Private::progressChanged(qreal progress)
{
    InstallJob *job = qobject_cast<InstallJob *>(q->sender());

    if (!job || !installJobsIndexes.contains(job)) {
        return;
    }

    emit q->dataChanged(installJobsIndexes[job], installJobsIndexes[job]);
}

void UpdatedAssetsModel::Private::jobDestroyed(QObject *obj)
{
    //WARNING: this static_cast is safe because nobody will access the job* pointer
    // don't ever access it in this function!
    InstallJob *job = static_cast<InstallJob *>(obj);

    if (installJobsIndexes.contains(job)) {
        installJobs.remove(installJobsIndexes.value(job));
        installJobsIndexes.remove(job);
    }
}


UpdatedAssetsModel::UpdatedAssetsModel(QObject *parent)
    : d(new Private(this))
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "DisplayRole");
    roles.insert(Qt::DecorationRole, "DecorationRole");
    QMetaEnum e = metaObject()->enumerator(metaObject()->indexOfEnumerator("DisplayRoles"));
    for (int i = 0; i < e.keyCount(); ++i) {
        roles.insert(e.value(i), e.key(i));
    }
    setRoleNames(roles);

    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SIGNAL(countChanged()));
    connect(this, SIGNAL(modelReset()),
            this, SIGNAL(countChanged()));

    connect(InstallJobsModel::self(), SIGNAL(jobAdded(Bodega::AssetInfo, Bodega::InstallJob *)),
            this, SLOT(jobAdded(Bodega::AssetInfo, Bodega::InstallJob *)));

    QMetaObject::invokeMethod(this, "reload", Qt::QueuedConnection);
}

UpdatedAssetsModel::~UpdatedAssetsModel()
{
    delete d;
}

UpdatedAssetsModel* UpdatedAssetsModel::self()
{
    if (!UpdatedAssetsModel::Private::s_self) {
        UpdatedAssetsModel::Private::s_self = new UpdatedAssetsModel;
    }

    return UpdatedAssetsModel::Private::s_self;
}

void UpdatedAssetsModel::reload()
{
    beginResetModel();
    d->assets.clear();
    d->sessionIndexes.clear();
    endResetModel();

    if (!d->db.isValid()) {
        const QString dbPath = Bodega::AssetHandler::updateDatabasePath();
        if (QFile::exists(dbPath)) {
            d->db = Bodega::AssetHandler::updateDatabase();
        }

        if (!d->db.isValid()) {
            return;
        }
    }

    QSqlQuery query(d->db);
    query.prepare(QLatin1String("SELECT store, warehouse, asset FROM assets WHERE updated ORDER BY warehouse, store"));
    if (!query.exec()) {
        qDebug() << "Failed to get updated asset list from database:" << query.lastError();
        return;
    }

    QString currentStore;
    QString currentWarehouse;
    QSqlRecord record = query.record();
    const int storeCol = record.indexOf(QLatin1String("store"));
    const int warehouseCol = record.indexOf(QLatin1String("warehouse"));
    const int assetCol = record.indexOf(QLatin1String("asset"));
    QStringList assetIds;
    while (query.next()) {
        const QString store = query.value(storeCol).toString();
        const QString warehouse = query.value(warehouseCol).toString();
        const QString asset = query.value(assetCol).toString();

        if (currentStore != store || currentWarehouse != warehouse) {
            if (!currentStore.isEmpty()) {
                d->fetchBriefs(currentStore, currentWarehouse, assetIds);
                assetIds.clear();
            }

            currentStore = store;
            currentWarehouse = warehouse;
        }

        assetIds.append(asset);
    }

    if (!assetIds.isEmpty()) {
        d->fetchBriefs(currentStore, currentWarehouse, assetIds);
    }
}

void UpdatedAssetsModel::updateAll()
{
    int i = 0;
    foreach (const AssetInfo &info, d->assets) {
        //d->sessionIndexes[i]->install(d->sessionIndexes[i]->assetOperations(info.id));
        
        //FIXME: nested loop, evil++
        QMapIterator<int, Bodega::Session *> it(d->sessionIndexes);
        it.toBack();
        while (it.hasPrevious()) {
            it.previous();
            if (it.key() <= i) {
                Session *session = it.value();
                AssetOperations *operation = session->assetOperations(info.id);
                //FIXME: make all of this asynchronous
                QEventLoop eventLoop;
                connect(operation, SIGNAL(ready()),
                        &eventLoop, SLOT(quit()));
                eventLoop.exec();
                session->install(operation);
            }
        }
        
        ++i;
    }
}

bool UpdatedAssetsModel::containsAsset(const QString &assetId)
{
    //FIXME: get rid of this linear scan
    foreach (const AssetInfo &existingInfo, d->assets) {
        if (existingInfo.id == assetId) {
            return true;
        }
    }
    return false;
}

int UpdatedAssetsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant UpdatedAssetsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > d->assets.size()) {
        return QVariant();
    }

    const AssetInfo &info = d->assets.at(index.row());
    switch (role) {
        case Qt::DisplayRole:
            return info.name;
        case AssetIdRole:
            return info.id;
        case AssetNameRole:
            return info.name;
        case AssetVersionRole:
            return info.version;
        case AssetFilenameRole:
            return info.filename;
        case AssetLicenseRole:
            return info.license;
        case AssetLicenseTextRole:
            return info.licenseText;
        case AssetPartnerIdRole:
            return info.partnerId;
        case AssetPartnerNameRole:
            return info.partnerName;
        case AssetSizeRole:
            return info.size;
        case ImageTinyRole:
            return info.images.value(Bodega::ImageTiny);
        case ImageSmallRole:
            return info.images.value(Bodega::ImageSmall);
        case ImageMediumRole:
            return info.images.value(Bodega::ImageMedium);
        case ImageLargeRole:
            return info.images.value(Bodega::ImageLarge);
        case ImageHugeRole:
            return info.images.value(Bodega::ImageHuge);
        case ImagePreviewsRole:
            return info.images.value(Bodega::ImagePreviews);
        case SessionRole: {
            QMapIterator<int, Bodega::Session *> it(d->sessionIndexes);
            it.toBack();
            while (it.hasPrevious()) {
                it.previous();
                if (it.key() <= index.row()) {
                    return QVariant::fromValue(it.value());
                }
            }
            return QVariant();
        }
        case ProgressRole: {
            if (d->installJobs.contains(index)) {
                return d->installJobs[index]->progress();
            } else {
                return QVariant();
            }
        }
            break;
        default:
            return QVariant();
    }
}

bool UpdatedAssetsModel::hasChildren(const QModelIndex &parent) const
{
    return false;
}

QModelIndex UpdatedAssetsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QModelIndex UpdatedAssetsModel::parent(const QModelIndex &index) const
{
    return QModelIndex();

}

int UpdatedAssetsModel::rowCount(const QModelIndex &parent) const
{
    return d->assets.size();
}

}

#include "updatedassetsmodel.moc"

