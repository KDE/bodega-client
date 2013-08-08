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

#include "assetoperations_p.h"
#include "ratingattributesjob.h"

#include <QtCore/QMetaEnum>

namespace Bodega {

QHash<QString, QList<RatingAttributes> > AssetOperations::RatingsModel::s_ratingAttributesByAssetType;

AssetOperations::RatingsModel::RatingsModel(QObject *parent)
    : QAbstractItemModel(parent),
    m_session(0)
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
    connect(this, SIGNAL(assetJobChanged()),
            this, SLOT(fetchRatingAttributes()));
}

AssetOperations::RatingsModel::~RatingsModel()
{
}

AssetJob *AssetOperations::RatingsModel::assetJob() const
{
    return m_assetJob;
}

void AssetOperations::RatingsModel::setAssetJob(AssetJob *assetJob)
{
    if (!assetJob) {
        return;
    }

    m_assetJob = assetJob;
    emit assetJobChanged();
}

int AssetOperations::RatingsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant AssetOperations::RatingsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_ratingAttributes.count()) {
        return QVariant();
    }

    switch (role) {
        case Name: {
            return m_ratingAttributes.at(index.row()).name;
        }
        case AttributeId: {
            return m_ratingAttributes.at(index.row()).id;
        }
        case LowDesc: {
            return m_ratingAttributes.at(index.row()).lowDesc;
        }
        case HighDesc: {
            return m_ratingAttributes.at(index.row()).highDesc;
        }
        case AssetType: {
            return m_ratingAttributes.at(index.row()).assetType;
        }
        case RatingsCount: {
            return findRatingsCount(m_ratingAttributes.at(index.row()).id);
        }
        case AverageRating: {
            return findAverageRating(m_ratingAttributes.at(index.row()).id);
        }
        case AllRatings: {
            return m_allRatings;
        }
        default: {
            return QVariant();
        }
    }
}

Qt::ItemFlags AssetOperations::RatingsModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool AssetOperations::RatingsModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return false;
}

QVariant AssetOperations::RatingsModel::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    return QVariant();
}

QModelIndex AssetOperations::RatingsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0) {
        return QModelIndex();
    }

    if (row < 0 || row >= m_ratingAttributes.count()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QMap<int, QVariant> AssetOperations::RatingsModel::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex AssetOperations::RatingsModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int AssetOperations::RatingsModel::rowCount(const QModelIndex &parent) const
{
    return m_ratingAttributes.size();
}

void AssetOperations::RatingsModel::setSession(Session *session)
{
    if (session == m_session) {
        return;
    }

    if (m_session) {
        //not connected directly, so disconnect everything
        m_session->disconnect(this);
    }
    m_session = session;
}

Session *AssetOperations::RatingsModel::session() const
{
    return m_session;
}

QString AssetOperations::RatingsModel::findAverageRating(const QString &ratingAttributeId) const
{
    foreach(const AssetInfo::AssetInfoRatings &info, m_assetJob->info().ratings) {
        if (ratingAttributeId == info.attributeId) {
            return info.averageRating;
        }
    }
    return QString();
}

QString AssetOperations::RatingsModel::findRatingsCount(const QString &ratingAttributeId) const
{
    foreach(const AssetInfo::AssetInfoRatings &info, m_assetJob->info().ratings) {
        if (ratingAttributeId == info.attributeId) {
            return info.ratingsCount;
        }
    }
    return QString();
}

int AssetOperations::RatingsModel::allRatings()
{
    foreach(const AssetInfo::AssetInfoRatings &info, m_assetJob->info().ratings) {
        m_allRatings += info.ratingsCount.toInt();
    }
    return m_allRatings;
}

void AssetOperations::RatingsModel::fetchRatingAttributes()
{
    beginResetModel();
    m_allRatings = 0;
    m_ratingAttributes.clear();
    endResetModel();

    const QString contentType = m_assetJob->contentType();

    if (!contentType.isEmpty()) {
        if (RatingsModel::s_ratingAttributesByAssetType.contains(contentType)) {
            m_ratingAttributes = RatingsModel::s_ratingAttributesByAssetType[contentType];

            const int begin = 0;
            const int end = qMax(begin, m_ratingAttributes.count() -1);
            beginInsertRows(QModelIndex(), begin, end);
            allRatings();
            endInsertRows();
        } else {
            RatingAttributesJob *job = m_session->listRatingAttributes(m_assetJob->assetId());
            connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
                this, SLOT(ratingAttributesJobFinished(Bodega::NetworkJob *)));
        }
    }
}

void AssetOperations::RatingsModel::ratingAttributesJobFinished(Bodega::NetworkJob *job)
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
    const int end = qMax(begin, ratingAttributesJob->ratingAttributes().count() -1);

    beginInsertRows(QModelIndex(), begin, end);
    m_ratingAttributes = ratingAttributesJob->ratingAttributes();
    RatingsModel::s_ratingAttributesByAssetType.insert(m_assetJob->contentType(), m_ratingAttributes);
    allRatings();
    endInsertRows();
}

}

#include "assetoperations_p.moc"
