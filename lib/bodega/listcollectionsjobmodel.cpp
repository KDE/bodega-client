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

#include "listballotsjobmodel.h"

#include "listballotsjob.h"
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

class ListBallotsJobModel::Private {
public:
    Private(ListBallotsJobModel *parent);

    ListBallotsJobModel *q;
    Session *session;
    QList<BallotInfo> ballots;
    bool hasMore;
    int fetchedBallots;
    void fetchInitialCollections();
    void ballotsJobFinished(Bodega::NetworkJob *job);
};

ListBallotsJobModel::Private::Private(ListBallotsJobModel *parent)
    : q(parent),
      session(0),
      hasMore(false)
{
}

void ListBallotsJobModel::Private::fetchInitialCollections()
{
    ListBallotsJob *job = session->listBallots(0, DEFAULT_PAGE_SIZE);

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(ballotsJobFinished(Bodega::NetworkJob *)));
}

void ListBallotsJobModel::Private::ballotsJobFinished(Bodega::NetworkJob *job)
{
    ListBallotsJob *ballotJob = qobject_cast<ListBallotsJob*>(job);

    if (!ballotJob) {
        return;
    }

    ballotJob->deleteLater();

    if (ballotJob->failed()) {
        return;
    }

    hasMore = ballotJob->hasMoreBallots();

    if (ballotJob->ballots().isEmpty()) {
        return;
    }

    fetchedBallots += ballotJob->ballots().size();

    //clear the model and our data
    q->beginResetModel();
    ballots.clear();
    q->endResetModel();

    const int begin = ballotJob->offset();
    const int end = qMax(begin, ballotJob->ballots().count() + begin - 1);
    q->beginInsertRows(QModelIndex(), begin, end);
    ballots = ballotJob->ballots();
    q->endInsertRows();
}

ListBallotsJobModel::ListBallotsJobModel(QObject *parent)
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

ListBallotsJobModel::~ListBallotsJobModel()
{
    delete d;
}

bool ListBallotsJobModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return false;
    }

    return d->hasMore;
}

int ListBallotsJobModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant ListBallotsJobModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= d->ballots.count()) {
        return QVariant();
    }

    switch (role) {
        case CollectionName: {
            return d->ballots.at(index.row()).name;
        }
        case CollectionId: {
            return d->ballots.at(index.row()).id;
        }
        default: {
            return QVariant();
        }
    }
}

void ListBallotsJobModel::fetchMore(const QModelIndex &parent)
{
    if (!parent.isValid() || !d->session || !canFetchMore(parent) ||
        !d->session->isAuthenticated()) {
        return;
    }

    ListBallotsJob *job = d->session->listBallots(d->ballots.count(), DEFAULT_PAGE_SIZE);

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            this, SLOT(ballotsJobFinished(Bodega::NetworkJob *)));
}

Qt::ItemFlags ListBallotsJobModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool ListBallotsJobModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return false;
}

QVariant ListBallotsJobModel::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    return QVariant();
}

QModelIndex ListBallotsJobModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0) {
        return QModelIndex();
    }

    if (row < 0 || row >= d->ballots.count()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QMap<int, QVariant> ListBallotsJobModel::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex ListBallotsJobModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int ListBallotsJobModel::rowCount(const QModelIndex &parent) const
{
    return d->ballots.size();
}

void ListBallotsJobModel::setSession(Session *session)
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

Session *ListBallotsJobModel::session() const
{
    return d->session;
}

}

#include "listballotsjobmodel.moc"
