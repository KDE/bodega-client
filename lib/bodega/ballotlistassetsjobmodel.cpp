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

#include "ballotlistassetsjobmodel.h"

#include "ballotlistassetsjob.h"
#include "session.h"
#include "networkjob.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QMetaEnum>
#include <QDebug>
namespace Bodega
{

static const int DEFAULT_PAGE_SIZE = 50;

class BallotListAssetsJobModel::Private {
public:
    Private(BallotListAssetsJobModel *parent);

    BallotListAssetsJobModel *q;
    Session *session;
    bool hasMore;
    int fetchedAssets;
    void fetchAssets();
    void assetsJobFinished(Bodega::NetworkJob *job);
    QString collectionId;
    QList<AssetInfo> assets;
};

BallotListAssetsJobModel::Private::Private(BallotListAssetsJobModel *parent)
    : q(parent),
      session(0),
      hasMore(false)
{
}

void BallotListAssetsJobModel::Private::fetchAssets()
{
    //this method will be called when the collectionId has changed inside from QML,
    //so we will use the 0(zero) as our offset, because we don't do any incremental stuff
    //here. For incremental retrieve of out data we have the fetchMore
    BallotListAssetsJob *job = session->collectionListAssets(collectionId, 0, DEFAULT_PAGE_SIZE);

    q->beginResetModel();
    assets.clear();
    hasMore = false;
    q->endResetModel();

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(assetsJobFinished(Bodega::NetworkJob *)));
}

void BallotListAssetsJobModel::Private::assetsJobFinished(Bodega::NetworkJob *job)
{
    BallotListAssetsJob *ballotJob = qobject_cast<BallotListAssetsJob*>(job);

    if (!ballotJob) {
        return;
    }

    ballotJob->deleteLater();

    if (ballotJob->failed()) {
        return;
    }

    hasMore = ballotJob->hasMoreAssets();
    fetchedAssets += ballotJob->assets().count();

    const int begin = ballotJob->offset();
    const int end = qMax(begin, assets.count() + begin -1);
    q->beginInsertRows(QModelIndex(), begin, end);
    assets = ballotJob->assets();
    q->endInsertRows();
}

BallotListAssetsJobModel::BallotListAssetsJobModel(QObject *parent)
    : QAbstractItemModel(parent),
      d(new Private(this))
{
    // set the role names based on the values of the DisplayRoles enum for
    //  the sake of QML
    QHash<int, QByteArray> roles;
    roles.insert(AssetIdRole, "AssetIdRole");
    roles.insert(AssetNameRole, "AssetNameRole");
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

BallotListAssetsJobModel::~BallotListAssetsJobModel()
{
    delete d;
}

QString BallotListAssetsJobModel::collectionId() const
{
    return d->collectionId;
}

void BallotListAssetsJobModel::setCollectionId(const QString& collectionId)
{
    d->collectionId = collectionId;

    emit collectionIdChanged();
}

bool BallotListAssetsJobModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return false;
    }

    return d->hasMore;
}

int BallotListAssetsJobModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant BallotListAssetsJobModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= d->assets.count()) {
        return QVariant();
    }

    switch (role) {
        case AssetIdRole: {
            qDebug() << d->assets.at(index.row()).id;
            return d->assets.at(index.row()).id;
        }
        case AssetNameRole: {
                                qDebug() << d->assets.at(index.row()).name;
            return d->assets.at(index.row()).name;
        }
        default: {
            return QVariant();
        }
    }
}

void BallotListAssetsJobModel::fetchMore(const QModelIndex &parent)
{
    if (!parent.isValid() || !d->session || !canFetchMore(parent) ||
        !d->session->isAuthenticated()) {
        return;
    }

    BallotListAssetsJob *job = d->session->collectionListAssets(d->collectionId, d->assets.count(), DEFAULT_PAGE_SIZE);

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            this, SLOT(assetsJobFinished(Bodega::NetworkJob *)));
}

Qt::ItemFlags BallotListAssetsJobModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool BallotListAssetsJobModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return false;
}

QVariant BallotListAssetsJobModel::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    return QVariant();
}

QModelIndex BallotListAssetsJobModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0) {
        return QModelIndex();
    }

    if (row < 0 || row >= d->assets.count()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QMap<int, QVariant> BallotListAssetsJobModel::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex BallotListAssetsJobModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int BallotListAssetsJobModel::rowCount(const QModelIndex &parent) const
{
    return d->assets.size();
}

void BallotListAssetsJobModel::setSession(Session *session)
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

Session *BallotListAssetsJobModel::session() const
{
    return d->session;
}

}

#include "ballotlistassetsjobmodel.moc"
