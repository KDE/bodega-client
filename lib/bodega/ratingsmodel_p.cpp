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

#include "ratingsmodel_p.h"
#include "ratingattributesjob.h"

#include <QtCore/QMetaEnum>
#include <QDebug>

namespace Bodega {

QHash<QString, QList<RatingAttributes> > RatingsModel::s_ratingAttributesByAssetType;

RatingsModel::RatingsModel(QObject *parent)
    : QAbstractItemModel(parent),
      m_session(0),
      m_ratingsCount(0),
      m_assetJob(0)
{
    // set the role names based on the values of the DisplayRoles enum for
    //  the sake of QML
    QHash<int, QByteArray> roles;

    QMetaEnum e = metaObject()->enumerator(metaObject()->indexOfEnumerator("DisplayRoles"));
    for (int i = 0; i < e.keyCount(); ++i) {
        roles.insert(e.value(i), e.key(i));
    }
    setRoleNames(roles);
}

RatingsModel::~RatingsModel()
{
}

AssetJob *RatingsModel::assetJob() const
{
    return m_assetJob;
}

void RatingsModel::setAssetJob(AssetJob *assetJob)
{
    if (!assetJob) {
        return;
    }

    beginResetModel();

    m_assetJob = assetJob;
    m_contentType = m_assetJob->contentType();
    m_assetInfo = m_assetJob->info();

    if (!m_contentType.isEmpty()) {
        if (s_ratingAttributesByAssetType.contains(m_contentType)) {
            m_ratingAttributes = RatingsModel::s_ratingAttributesByAssetType[m_contentType];
        } else {
            m_ratingAttributes.clear();
            RatingAttributesJob *job = m_session->listRatingAttributes(m_assetJob->assetId());
            connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
                    this, SLOT(ratingAttributesJobFinished(Bodega::NetworkJob *)));
        }
    }

    endResetModel();

    int ratingsCount = m_ratingsCount;
    m_ratingsCount = 0;
    foreach (const AssetInfo::AssetInfoRatings &info, m_assetInfo.ratings) {
        m_ratingsCount = qMax(m_ratingsCount, info.ratingsCount.toInt());
    }

    if (ratingsCount != m_ratingsCount) {
        emit ratingsCountChanged();
    }
}

int RatingsModel::ratingsCount() const
{
    return m_ratingsCount;
}

int RatingsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant RatingsModel::data(const QModelIndex &index, int role) const
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
        default: {
            return QVariant();
        }
    }
}

Qt::ItemFlags RatingsModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool RatingsModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return false;
}

QVariant RatingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

QModelIndex RatingsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0) {
        return QModelIndex();
    }

    if (row < 0 || row >= m_ratingAttributes.count()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QMap<int, QVariant> RatingsModel::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex RatingsModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int RatingsModel::rowCount(const QModelIndex &parent) const
{
    return m_ratingAttributes.size();
}

void RatingsModel::setSession(Session *session)
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

Session *RatingsModel::session() const
{
    return m_session;
}

QString RatingsModel::findAverageRating(const QString &ratingAttributeId) const
{
    foreach(const AssetInfo::AssetInfoRatings &info, m_assetInfo.ratings) {
        if (ratingAttributeId == info.attributeId) {
            return info.averageRating;
        }
    }
    return QString();
}

QString RatingsModel::findRatingsCount(const QString &ratingAttributeId) const
{
    foreach(const AssetInfo::AssetInfoRatings &info, m_assetInfo.ratings) {
        if (ratingAttributeId == info.attributeId) {
            return info.ratingsCount;
        }
    }
    return QString();
}

void RatingsModel::ratingAttributesJobFinished(Bodega::NetworkJob *job)
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
    s_ratingAttributesByAssetType.insert(m_contentType, m_ratingAttributes);
    endInsertRows();
}

}

#include "ratingsmodel_p.moc"
