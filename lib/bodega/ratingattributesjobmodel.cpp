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

#include "ratingattributesjobmodel.h"

#include "ratingattributesjob.h"
#include "session.h"
#include "networkjob.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QMetaEnum>
#include <QDebug>
namespace Bodega
{

class RatingAttributesJobModel::Private {
public:
    Private(RatingAttributesJobModel *parent);

    RatingAttributesJobModel *q;
    Session *session;
    void fetchRatingAttributes();
    void ratingAttributesJobFinished(Bodega::NetworkJob *job);
    void assetJobFinished(Bodega::NetworkJob *job);

    QString findAverageRating(const QString ratingAttributeId) const;
    QString findRatingsCount(const QString ratingAttributeId) const;

    QString assetId;
    QList<RatingAttributes> ratingAttributes;
    AssetInfo assetInfo;
};

RatingAttributesJobModel::Private::Private(RatingAttributesJobModel *parent)
    : q(parent),
      session(0)
{
}

void RatingAttributesJobModel::Private::fetchRatingAttributes()
{
    RatingAttributesJob *job = session->listRatingAttributes(assetId);

    q->beginResetModel();
    ratingAttributes.clear();
    assetInfo.clear();
    q->endResetModel();

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(ratingAttributesJobFinished(Bodega::NetworkJob *)));
}

void RatingAttributesJobModel::Private::ratingAttributesJobFinished(Bodega::NetworkJob *job)
{
    RatingAttributesJob *ratingAttributesJob = qobject_cast<RatingAttributesJob*>(job);

    if (!ratingAttributesJob) {
        return;
    }

    ratingAttributesJob->deleteLater();

    if (ratingAttributesJob->failed()) {
        return;
    }

    const int begin = 0;
    const int end = qMax(begin, ratingAttributes.count() + begin -1);
    q->beginInsertRows(QModelIndex(), begin, end);
    ratingAttributes = ratingAttributesJob->ratingAttributes();

    AssetJob *assetJob = session->asset(assetId, AssetJob::Ratings);
    connect(assetJob, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(assetJobFinished(Bodega::NetworkJob *)));
}

void RatingAttributesJobModel::Private::assetJobFinished(Bodega::NetworkJob *job)
{
    AssetJob *assetJob = qobject_cast<AssetJob*>(job);

    if (!assetJob) {
        return;
    }

    assetJob->deleteLater();

    if (assetJob->failed()) {
        q->endInsertRows();
        return;
    }
    assetInfo = assetJob->info();
    q->endInsertRows();
}

QString RatingAttributesJobModel::Private::findAverageRating(const QString ratingAttributeId) const
{
    foreach(const AssetInfo::AssetInfoRatings &info, assetInfo.ratings) {
        if (ratingAttributeId == info.attributeId) {
            return info.averageRating;
        }
    }
    return QString();
}

QString RatingAttributesJobModel::Private::findRatingsCount(const QString ratingAttributeId) const
{
    foreach(const AssetInfo::AssetInfoRatings &info, assetInfo.ratings) {
        if (ratingAttributeId == info.attributeId) {
            return info.ratingsCount;
        }
    }
    return QString();
}

RatingAttributesJobModel::RatingAttributesJobModel(QObject *parent)
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
            this, SLOT(fetchRatingAttributes()));
}

RatingAttributesJobModel::~RatingAttributesJobModel()
{
    delete d;
}

QString RatingAttributesJobModel::assetId() const
{
    return d->assetId;
}

void RatingAttributesJobModel::setAssetId(const QString& assetId)
{
    d->assetId = assetId;
    emit assetIdChanged();
}

int RatingAttributesJobModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant RatingAttributesJobModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= d->ratingAttributes.count()) {
        return QVariant();
    }

    switch (role) {
        case Name: {
            return d->ratingAttributes.at(index.row()).name;
        }
        case LowDesc: {
            return d->ratingAttributes.at(index.row()).lowDesc;
        }
        case HighDesc: {
            return d->ratingAttributes.at(index.row()).highDesc;
        }
        case AssetType: {
            return d->ratingAttributes.at(index.row()).assetType;
        }
        case RatingsCount: {
            return d->findRatingsCount(d->ratingAttributes.at(index.row()).id);
        }
        case AverageRating: {
            return d->findAverageRating(d->ratingAttributes.at(index.row()).id);
        }

        default: {
            return QVariant();
        }
    }
}

Qt::ItemFlags RatingAttributesJobModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool RatingAttributesJobModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return false;
}

QVariant RatingAttributesJobModel::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    return QVariant();
}

QModelIndex RatingAttributesJobModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0) {
        return QModelIndex();
    }

    if (row < 0 || row >= d->ratingAttributes.count()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QMap<int, QVariant> RatingAttributesJobModel::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex RatingAttributesJobModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int RatingAttributesJobModel::rowCount(const QModelIndex &parent) const
{
    return d->ratingAttributes.size();
}

void RatingAttributesJobModel::setSession(Session *session)
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

Session *RatingAttributesJobModel::session() const
{
    return d->session;
}

}

#include "ratingattributesjobmodel.moc"
