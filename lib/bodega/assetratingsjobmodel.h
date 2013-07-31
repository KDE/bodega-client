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

#ifndef BODEGA_ASSETRATINGSJOB_MODEL_H
#define BODEGA_ASSETRATINGSJOB_MODEL_H

#include <bodega/globals.h>

#include <QtCore/QAbstractItemModel>

namespace Bodega {

    class Session;

    class BODEGA_EXPORT AssetRatingsJobModel : public QAbstractItemModel
    {
        Q_OBJECT
        Q_ENUMS(DisplayRoles)
        Q_PROPERTY(int count READ count NOTIFY countChanged)
        Q_PROPERTY(QString assetId READ assetId WRITE setAssetId NOTIFY assetIdChanged)

    public:
        enum DisplayRoles {
            AttributeId = Qt::UserRole + 100,
            PersonId = Qt::UserRole + 101,
            Rating = Qt::UserRole + 102
        };

        AssetRatingsJobModel(QObject *parent = 0);
        ~AssetRatingsJobModel();

        QString assetId() const;
        void setAssetId(const QString& collectionId);

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
        void assetIdChanged();

    private:
        class Private;
        friend class Private;
        Private * const d;
        Q_PRIVATE_SLOT(d, void ratingsJobFinished(Bodega::NetworkJob *))
        Q_PRIVATE_SLOT(d, void fetchRatings())
    };

}

#endif
