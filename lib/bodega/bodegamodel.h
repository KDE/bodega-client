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

#ifndef BODEGA_MODEL_H
#define BODEGA_MODEL_H

#include <bodega/globals.h>

#include <QtCore/QAbstractItemModel>

namespace Bodega {

    class Session;

    class BODEGA_EXPORT Model : public QAbstractItemModel
    {
        Q_OBJECT
        Q_PROPERTY(QString topChannel READ topChannel WRITE setTopChannel NOTIFY topChannelChanged)
        Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)
        Q_ENUMS(DisplayRoles)

    public:
        enum DisplayRoles {
            ChannelIdRole = Qt::UserRole + 1,
            ChannelNameRole,
            ChannelDescriptionRole,
            ChannelAssetCountRole,

            ImageTinyRole = Qt::UserRole + 50,
            ImageSmallRole,
            ImageMediumRole,
            ImageLargeRole,
            ImageHugeRole,
            ImagePreviewsRole,

            AssetIdRole = Qt::UserRole + 100,
            AssetLicenseRole,
            AssetLicenseTextRole,
            AssetPartnerIdRole,
            AssetPartnerNameRole,
            AssetNameRole,
            AssetVersionRole,
            AssetFilenameRole,
            AssetDescriptionRole,
            AssetPointsRole,
            AssetSizeRole
        };

        Model(QObject *parent = 0);
        ~Model();

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

        void setSession(Session *session);
        Session *session() const;

        void setTopChannel(const QString &topChannel);
        QString topChannel() const;

        void setSearchQuery(const QString &query);
        QString searchQuery() const;

    Q_SIGNALS:
        void topChannelChanged();
        void searchQueryChanged();

    private:
        class Private;
        friend class Private;
        Private * const d;
        Q_PRIVATE_SLOT(d, void channelsJobFinished(Bodega::NetworkJob *))
        Q_PRIVATE_SLOT(d, void reloadFromNetwork())
    };

}

#endif
