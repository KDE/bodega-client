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


#ifndef ASSETOPERATIONINTERFACEPRIVATE_H
#define ASSETOPERATIONINTERFACEPRIVATE_H

#include <QObject>

#include <bodega/globals.h>
//#include <bodega/assetoperations.h>
#include "assetoperations.h"
#include <QAbstractItemModel>

namespace Bodega {

    class AssetJob;

class RatingsModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_ENUMS(DisplayRoles)

    public:
        enum DisplayRoles {
            Name = Qt::UserRole + 100,
            LowDesc = Qt::UserRole + 101,
            HighDesc = Qt::UserRole + 102,
            AssetType = Qt::UserRole + 103,
            RatingsCount = Qt::UserRole + 104,
            AverageRating = Qt::UserRole + 105,
            AllRatings = Qt::UserRole + 106,
            AttributeId = Qt::UserRole + 107
        };

        RatingsModel(QObject *parent = 0);
        ~RatingsModel();

        AssetJob *assetJob() const;
        void setAssetJob(AssetJob *assetJob);

        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const;
        QMap<int, QVariant> itemData(const QModelIndex &index) const;
        QModelIndex parent(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int count() const {return rowCount();}

        void setSession(Session *session);
        Session *session() const;

    Q_SIGNALS:
        void countChanged();

    private Q_SLOTS:
        void ratingAttributesJobFinished(Bodega::NetworkJob *);
    private:
        AssetJob *m_assetJob;
        Session *m_session;

        QList<RatingAttributes> m_ratingAttributes;
        int m_allRatings;
        QString findRatingsCount(const QString &foo) const;
        QString findAverageRating(const QString &foo) const;
        int allRatings();
        QString m_contentType;
        AssetInfo m_assetInfo;

        static QHash<QString, QList<RatingAttributes> > s_ratingAttributesByAssetType;
};

}

#endif
