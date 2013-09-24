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

#include "collectionassetsmodel.h"

#include "collectionlistassetsjob.h"
#include "session.h"
#include "networkjob.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QMetaEnum>
#include <QDebug>
namespace Bodega
{

static const int DEFAULT_PAGE_SIZE = 50;

class CollectionAssetsModel::Private {
public:
    Private(CollectionAssetsModel *parent);

    CollectionAssetsModel *q;
    Session *session;
    bool hasMore;
    int fetchedAssets;
    void fetchAssets();
    void assetsJobFinished(Bodega::NetworkJob *job);
    QString collectionId;
    QList<AssetInfo> assets;
};

CollectionAssetsModel::Private::Private(CollectionAssetsModel *parent)
    : q(parent),
      session(0),
      hasMore(false)
{
}

void CollectionAssetsModel::Private::fetchAssets()
{
    //this method will be called when the collectionId has changed inside from QML,
    //so we will use the 0(zero) as our offset, because we don't do any incremental stuff
    //here. For incremental retrieve of out data we have the fetchMore
    collectionListAssetsJob *job = session->collectionListAssets(collectionId, 0, DEFAULT_PAGE_SIZE);

    q->beginResetModel();
    assets.clear();
    hasMore = false;
    q->endResetModel();

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(assetsJobFinished(Bodega::NetworkJob *)));
}

void CollectionAssetsModel::Private::assetsJobFinished(Bodega::NetworkJob *job)
{
    collectionListAssetsJob *collectionJob = qobject_cast<collectionListAssetsJob*>(job);

    if (!collectionJob) {
        return;
    }

    collectionJob->deleteLater();

    if (collectionJob->failed()) {
        return;
    }

    hasMore = collectionJob->hasMoreAssets();
    fetchedAssets += collectionJob->assets().count();

    const int begin = collectionJob->offset();
    const int end = qMax(begin, assets.count() + begin -1);
    q->beginInsertRows(QModelIndex(), begin, end);
    assets = collectionJob->assets();
    q->endInsertRows();
}

CollectionAssetsModel::CollectionAssetsModel(QObject *parent)
    : QAbstractItemModel(parent),
      d(new Private(this))
{
    // set the role names based on the values of the DisplayRoles enum for
    //  the sake of QML
    QHash<int, QByteArray> roles;

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
    connect(this, SIGNAL(collectionIdChanged()),
            this, SLOT(fetchAssets()));
}

CollectionAssetsModel::~CollectionAssetsModel()
{
    delete d;
}

QString CollectionAssetsModel::collectionId() const
{
    return d->collectionId;
}

void CollectionAssetsModel::setCollectionId(const QString& collectionId)
{
    d->collectionId = collectionId;
    emit collectionIdChanged();
}

bool CollectionAssetsModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return false;
    }

    return d->hasMore;
}

int CollectionAssetsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant CollectionAssetsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= d->assets.count()) {
        return QVariant();
    }

    switch (role) {
        case AssetIdRole: {
            return d->assets.at(index.row()).id;
        }
        case AssetNameRole: {
            return d->assets.at(index.row()).name;
        }
        case AssetLicenseRole: {
            return d->assets.at(index.row()).license;
        }
        case AssetPartnerIdRole: {
            return d->assets.at(index.row()).partnerId;
        }
        case AssetPartnerNameRole: {
            return d->assets.at(index.row()).partnerName;
        }
        case AssetVersionRole: {
            return d->assets.at(index.row()).version;
        }
        case AssetFilenameRole: {
            return d->assets.at(index.row()).filename;
        }
        case AssetDescriptionRole: {
            return d->assets.at(index.row()).description;
        }
        case AssetPointsRole: {
            return d->assets.at(index.row()).points;
        }
        default: {
            return QVariant();
        }
    }
}

void CollectionAssetsModel::fetchMore(const QModelIndex &parent)
{
    if (!parent.isValid() || !d->session || !canFetchMore(parent) ||
        !d->session->isAuthenticated()) {
        return;
    }

    collectionListAssetsJob *job = d->session->collectionListAssets(d->collectionId, d->assets.count(), DEFAULT_PAGE_SIZE);

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            this, SLOT(assetsJobFinished(Bodega::NetworkJob *)));
}

Qt::ItemFlags CollectionAssetsModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool CollectionAssetsModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return false;
}

QVariant CollectionAssetsModel::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    return QVariant();
}

QModelIndex CollectionAssetsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0) {
        return QModelIndex();
    }

    if (row < 0 || row >= d->assets.count()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QMap<int, QVariant> CollectionAssetsModel::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex CollectionAssetsModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int CollectionAssetsModel::rowCount(const QModelIndex &parent) const
{
    return d->assets.size();
}

void CollectionAssetsModel::setSession(Session *session)
{
    if (session == d->session) {
        return;
    }

    if (d->session) {
        //not connected directly, so disconnect everything
        d->session->disconnect(this);
    }
    d->session = session;

    if (!d->session) {
        return;
    }
}

Session *CollectionAssetsModel::session() const
{
    return d->session;
}

}

#include "collectionassetsmodel.moc"
