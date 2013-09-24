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

#ifndef BODEGA_COLLECTIONLISTASSETSJOB_MODEL_H
#define BODEGA_COLLECTIONLISTASSETSJOB_MODEL_H

#include <bodega/globals.h>

#include <QtCore/QAbstractItemModel>

namespace Bodega {

    class Session;

    class BODEGA_EXPORT CollectionAssetsModel : public QAbstractItemModel
    {
        Q_OBJECT
        Q_ENUMS(DisplayRoles)
        Q_PROPERTY(int count READ count NOTIFY countChanged)
        Q_PROPERTY(QString collectionId READ collectionId WRITE setCollectionId NOTIFY collectionIdChanged)

    public:
        enum DisplayRoles {
            AssetIdRole = Qt::UserRole + 100,
            AssetLicenseRole = Qt::UserRole + 101,
            AssetPartnerIdRole = Qt::UserRole + 102,
            AssetPartnerNameRole = Qt::UserRole + 103,
            AssetNameRole = Qt::UserRole + 104,
            AssetVersionRole = Qt::UserRole + 105,
            AssetFilenameRole = Qt::UserRole + 106,
            AssetDescriptionRole = Qt::UserRole + 107,
            AssetPointsRole = Qt::UserRole + 108
        };

        CollectionAssetsModel(QObject *parent = 0);
        ~CollectionAssetsModel();

        QString collectionId() const;
        void setCollectionId(const QString& collectionId);

        //Invokable to make the view show a spinner when loading more
        Q_INVOKABLE bool canFetchMore(const QModelIndex &parent) const;

        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role) const;
        void fetchMore(const QModelIndex &parent);
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
        void collectionIdChanged();

    private:
        class Private;
        friend class Private;
        Private * const d;
        Q_PRIVATE_SLOT(d, void assetsJobFinished(Bodega::NetworkJob *))
        Q_PRIVATE_SLOT(d, void fetchAssets())
    };

}

#endif
