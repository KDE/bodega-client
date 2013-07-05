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

#include "listcollectionsjobmodel.h"

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

class ListcollectionsJobModel::Private {
public:
    Private(ListcollectionsJobModel *parent);

    ListcollectionsJobModel *q;
    Session *session;
    QList<collectionInfo> collections;
    bool hasMore;
    int fetchedcollections;
    void fetchInitialCollections();
    void collectionsJobFinished(Bodega::NetworkJob *job);
};

ListcollectionsJobModel::Private::Private(ListcollectionsJobModel *parent)
    : q(parent),
      session(0),
      hasMore(false)
{
}

void ListcollectionsJobModel::Private::fetchInitialCollections()
{
    ListcollectionsJob *job = session->listcollections(0, DEFAULT_PAGE_SIZE);

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(collectionsJobFinished(Bodega::NetworkJob *)));
}

void ListcollectionsJobModel::Private::collectionsJobFinished(Bodega::NetworkJob *job)
{
    ListcollectionsJob *collectionJob = qobject_cast<ListcollectionsJob*>(job);

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

ListcollectionsJobModel::ListcollectionsJobModel(QObject *parent)
    : QAbstractItemModel(parent),
      d(new Private(this))
{
    // set the role names based on the values of the DisplayRoles enum for
    //  the sake of QML
    QHash<int, QByteArray> roles;
    roles.insert(CollectionName, "CollectionName");
    roles.insert(CollectionId, "CollectionId");

    setRoleNames(roles);
qDebug() << "ctorrrrrrrrrrrrrr";
    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SIGNAL(countChanged()));
    connect(this, SIGNAL(modelReset()),
            this, SIGNAL(countChanged()));
}

ListcollectionsJobModel::~ListcollectionsJobModel()
{
    delete d;
}

bool ListcollectionsJobModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return false;
    }

    return d->hasMore;
}

int ListcollectionsJobModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant ListcollectionsJobModel::data(const QModelIndex &index, int role) const
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

void ListcollectionsJobModel::fetchMore(const QModelIndex &parent)
{
    if (!parent.isValid() || !d->session || !canFetchMore(parent) ||
        !d->session->isAuthenticated()) {
        return;
    }

    ListcollectionsJob *job = d->session->listcollections(d->collections.count(), DEFAULT_PAGE_SIZE);

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            this, SLOT(collectionsJobFinished(Bodega::NetworkJob *)));
}

Qt::ItemFlags ListcollectionsJobModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool ListcollectionsJobModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return false;
}

QVariant ListcollectionsJobModel::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    return QVariant();
}

QModelIndex ListcollectionsJobModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0) {
        return QModelIndex();
    }

    if (row < 0 || row >= d->collections.count()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QMap<int, QVariant> ListcollectionsJobModel::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex ListcollectionsJobModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int ListcollectionsJobModel::rowCount(const QModelIndex &parent) const
{
    return d->collections.size();
}

void ListcollectionsJobModel::setSession(Session *session)
{
    qDebug() << "sto setSession";
    if (session == d->session) {
        qDebug() << "why?";
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

Session *ListcollectionsJobModel::session() const
{
    return d->session;
}

}

#include "listcollectionsjobmodel.moc"
