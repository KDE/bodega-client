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
    QSqlDatabase db;
    void briefsJobFinished(Bodega::NetworkJob *);
    void reloadFromNetwork(bool);
    void fetchBriefs(const QString &store, const QString &warehouse, const QStringList &assets);
    void sessionAuthenticated(bool authed);
};

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
    assets.append(briefsJob->assets());
    q->endInsertRows();
}

void UpdatedAssetsModel::Private::fetchBriefs(const QString &store, const QString &warehouse, const QStringList &assets)
{
    const QString sessionId(SESSION_ID(store, warehouse));
    Bodega::Session *session = sessions.value(sessionId);
    if (!session) {
        pendingAssets[sessionId].append(assets);
        session = new Bodega::Session(q);
        session->setBaseUrl(warehouse);
        session->setStoreId(store);
        //FIXME!
        session->setUserName(QLatin1String("zack@kde.org"));
        session->setPassword(QLatin1String("zack"));
        sessions.insert(sessionId, session);
        connect(session, SIGNAL(authenticated(bool)), q, SLOT(sessionAuthenticated(bool)));
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
}

UpdatedAssetsModel::~UpdatedAssetsModel()
{
    delete d;
}

void UpdatedAssetsModel::reload()
{
    beginResetModel();
    d->assets.clear();
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
    QStringList assets;
    while (query.next()) {
        const QString store = query.value(storeCol).toString();
        const QString warehouse = query.value(warehouseCol).toString();
        const QString asset = query.value(assetCol).toString();

        if (currentStore != store || currentWarehouse != warehouse) {
            if (!currentStore.isEmpty()) {
                d->fetchBriefs(currentStore, currentWarehouse, assets);
                assets.clear();
            }

            currentStore = store;
            currentWarehouse = warehouse;
        }

        assets.append(asset);
    }

    if (!assets.isEmpty()) {
        d->fetchBriefs(currentStore, currentWarehouse, assets);
    }
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

