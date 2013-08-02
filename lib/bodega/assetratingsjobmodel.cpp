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

#include "assetratingsjobmodel.h"

#include "assetratingsjob.h"
#include "session.h"
#include "networkjob.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QMetaEnum>
#include <QDebug>
namespace Bodega
{

class AssetRatingsJobModel::Private {
public:
    Private(AssetRatingsJobModel *parent);

    AssetRatingsJobModel *q;
    Session *session;
    void fetchRatings();
    void ratingsJobFinished(Bodega::NetworkJob *job);
    QString assetId;
    QList<Ratings> ratings;
};

AssetRatingsJobModel::Private::Private(AssetRatingsJobModel *parent)
    : q(parent),
      session(0)
{
}

void AssetRatingsJobModel::Private::fetchRatings()
{
    AssetRatingsJob *job = session->assetRatings(assetId);

    q->beginResetModel();
    ratings.clear();
    q->endResetModel();

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(ratingsJobFinished(Bodega::NetworkJob *)));
}

void AssetRatingsJobModel::Private::ratingsJobFinished(Bodega::NetworkJob *job)
{
    AssetRatingsJob *assetRatingsJob = qobject_cast<AssetRatingsJob*>(job);

    if (!assetRatingsJob) {
        return;
    }

    assetRatingsJob->deleteLater();

    if (assetRatingsJob->failed()) {
        return;
    }

    const int begin = 0;
    const int end =  qMax(begin, assetRatingsJob->ratings().size() - 1);

    q->beginInsertRows(QModelIndex(), begin, end);
    ratings= assetRatingsJob->ratings();
    q->endInsertRows();
}

AssetRatingsJobModel::AssetRatingsJobModel(QObject *parent)
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
    connect(this, SIGNAL(assetIdChanged()),
            this, SLOT(fetchRatings()));
}

AssetRatingsJobModel::~AssetRatingsJobModel()
{
    delete d;
}

QString AssetRatingsJobModel::assetId() const
{
    return d->assetId;
}

void AssetRatingsJobModel::setAssetId(const QString& assetId)
{
    d->assetId = assetId;
    emit assetIdChanged();
}

int AssetRatingsJobModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant AssetRatingsJobModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= d->ratings.count()) {
        return QVariant();
    }

    switch (role) {
        case AttributeId: {
            return d->ratings.at(index.row()).attributeId;
        }
        case PersonId: {
            return d->ratings.at(index.row()).personId;
        }
        case Rating: {
            return d->ratings.at(index.row()).rating;
        }
        default: {
            return QVariant();
        }
    }
}

Qt::ItemFlags AssetRatingsJobModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool AssetRatingsJobModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return false;
}

QVariant AssetRatingsJobModel::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    return QVariant();
}

QModelIndex AssetRatingsJobModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0) {
        return QModelIndex();
    }

    if (row < 0 || row >= d->ratings.count()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QMap<int, QVariant> AssetRatingsJobModel::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex AssetRatingsJobModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int AssetRatingsJobModel::rowCount(const QModelIndex &parent) const
{
    return d->ratings.size();
}

void AssetRatingsJobModel::setSession(Session *session)
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

Session *AssetRatingsJobModel::session() const
{
    return d->session;
}

}

#include "assetratingsjobmodel.moc"
