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

#include "assetratingsmodel.h"

#include "assetratingsjob.h"
#include "ratingattributesjob.h"
#include "session.h"
#include "networkjob.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QMetaEnum>
#include <QDebug>
namespace Bodega
{

class AssetRatingsModel::Private {
public:
    Private(AssetRatingsModel *parent);

    AssetRatingsModel *q;
    Session *session;
    void fetchRatings();
    void ratingsJobFinished(Bodega::NetworkJob *job);
    void ratingAttributesJobFinished(Bodega::NetworkJob *job);
    QString assetId;
    QList<Ratings> ratings;
    QList<RatingAttributes> ratingAttributes;
    QString findAttributeName(const QString &attributeId) const;
};

AssetRatingsModel::Private::Private(AssetRatingsModel *parent)
    : q(parent),
      session(0)
{
}

void AssetRatingsModel::Private::fetchRatings()
{
    AssetRatingsJob *job = session->assetRatings(assetId);

    q->beginResetModel();
    ratings.clear();
    q->endResetModel();

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(ratingsJobFinished(Bodega::NetworkJob *)));
}

void AssetRatingsModel::Private::ratingsJobFinished(Bodega::NetworkJob *job)
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
    RatingAttributesJob *ratingAttributesJob = session->listRatingAttributes(assetId);
    connect(ratingAttributesJob, SIGNAL(jobFinished(Bodega::NetworkJob *)),
        q, SLOT(ratingAttributesJobFinished(Bodega::NetworkJob *)));
}

void AssetRatingsModel::Private::ratingAttributesJobFinished(Bodega::NetworkJob *job)
{
    RatingAttributesJob *ratingAttributesJob = qobject_cast<RatingAttributesJob*>(job);

    if (!ratingAttributesJob) {
        return;
    }

    ratingAttributesJob->deleteLater();

    if (ratingAttributesJob->failed()) {
        return;
    }
    ratingAttributes.append(ratingAttributesJob->ratingAttributes());
    q->endInsertRows();
}

QString AssetRatingsModel::Private::findAttributeName(const QString &attributeId) const
{
    foreach(const RatingAttributes &attribute, ratingAttributes) {
        if (attribute.id == attributeId) {
            return attribute.name;
        }
    }
    return QString();
}

AssetRatingsModel::AssetRatingsModel(QObject *parent)
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

AssetRatingsModel::~AssetRatingsModel()
{
    delete d;
}

QString AssetRatingsModel::assetId() const
{
    return d->assetId;
}

void AssetRatingsModel::setAssetId(const QString& assetId)
{
    d->assetId = assetId;
    emit assetIdChanged();
}

int AssetRatingsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant AssetRatingsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= d->ratings.count()) {
        return QVariant();
    }

    switch (role) {
        case AttributeId: {
            return d->ratings.at(index.row()).attributeId;
        }
        case AttributeName: {
            return d->findAttributeName(d->ratings.at(index.row()).attributeId);
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

Qt::ItemFlags AssetRatingsModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool AssetRatingsModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return false;
}

QVariant AssetRatingsModel::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    return QVariant();
}

QModelIndex AssetRatingsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0) {
        return QModelIndex();
    }

    if (row < 0 || row >= d->ratings.count()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QMap<int, QVariant> AssetRatingsModel::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex AssetRatingsModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int AssetRatingsModel::rowCount(const QModelIndex &parent) const
{
    return d->ratings.size();
}

void AssetRatingsModel::setSession(Session *session)
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

Session *AssetRatingsModel::session() const
{
    return d->session;
}

}

#include "assetratingsmodel.moc"
