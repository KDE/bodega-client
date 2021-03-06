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

#include "collectionsmodel.h"

#include "listcollectionsjob.h"
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

class CollectionsModel::Private {
public:
    Private(CollectionsModel *parent);

    CollectionsModel *q;
    Session *session;
    QList<CollectionInfo> collections;
    bool hasMore;
    int fetchedcollections;
    void fetchInitialCollections();
    void collectionsJobFinished(Bodega::NetworkJob *job);
};

CollectionsModel::Private::Private(CollectionsModel *parent)
    : q(parent),
      session(0),
      hasMore(false)
{
}

void CollectionsModel::Private::fetchInitialCollections()
{
    if (!session->isAuthenticated()) {
        return;
    }

    ListCollectionsJob *job = session->listcollections(0, DEFAULT_PAGE_SIZE);

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(collectionsJobFinished(Bodega::NetworkJob *)));
}

void CollectionsModel::Private::collectionsJobFinished(Bodega::NetworkJob *job)
{
    ListCollectionsJob *collectionJob = qobject_cast<ListCollectionsJob*>(job);

    if (!collectionJob) {
        return;
    }

    collectionJob->deleteLater();

    if (collectionJob->failed()) {
        return;
    }

    hasMore = collectionJob->hasMorecollections();

    if (collectionJob->collections().isEmpty()) {
        return;
    }

    fetchedcollections += collectionJob->collections().size();

    //clear the model and our data
    q->beginResetModel();
    collections.clear();
    q->endResetModel();

    const int begin = collectionJob->offset();
    const int end = qMax(begin, collectionJob->collections().count() + begin - 1);
    q->beginInsertRows(QModelIndex(), begin, end);
    collections = collectionJob->collections();
    q->endInsertRows();
}

CollectionsModel::CollectionsModel(QObject *parent)
    : QAbstractItemModel(parent),
      d(new Private(this))
{
    // set the role names based on the values of the DisplayRoles enum for
    //  the sake of QML
    QHash<int, QByteArray> roles;
    roles.insert(CollectionName, "CollectionName");
    roles.insert(CollectionId, "CollectionId");

    setRoleNames(roles);

    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SIGNAL(countChanged()));
    connect(this, SIGNAL(modelReset()),
            this, SIGNAL(countChanged()));
}

CollectionsModel::~CollectionsModel()
{
    delete d;
}

bool CollectionsModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return false;
    }

    return d->hasMore;
}

int CollectionsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant CollectionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= d->collections.count()) {
        return QVariant();
    }

    switch (role) {
        case CollectionName: {
            return d->collections.at(index.row()).name;
        }
        case CollectionId: {
            return d->collections.at(index.row()).id;
        }
        default: {
            return QVariant();
        }
    }
}

void CollectionsModel::fetchMore(const QModelIndex &parent)
{
    if (!parent.isValid() || !d->session || !canFetchMore(parent) ||
        !d->session->isAuthenticated()) {
        return;
    }

    ListCollectionsJob *job = d->session->listcollections(d->collections.count(), DEFAULT_PAGE_SIZE);

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            this, SLOT(collectionsJobFinished(Bodega::NetworkJob *)));
}

Qt::ItemFlags CollectionsModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool CollectionsModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return false;
}

QVariant CollectionsModel::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    return QVariant();
}

QModelIndex CollectionsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0) {
        return QModelIndex();
    }

    if (row < 0 || row >= d->collections.count()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QMap<int, QVariant> CollectionsModel::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex CollectionsModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int CollectionsModel::rowCount(const QModelIndex &parent) const
{
    return d->collections.size();
}

void CollectionsModel::setSession(Session *session)
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

    connect(d->session, SIGNAL(authenticated(bool)),
            this, SLOT(fetchInitialCollections()));
}

Session *CollectionsModel::session() const
{
    return d->session;
}

}

#include "collectionsmodel.moc"
