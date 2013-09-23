/*
 *   Copyright 2013 Coherent Theory LLC
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

#ifndef UPDATED_ASSETS_MODEL_H
#define UPDATED_ASSETS_MODEL_H

#include <bodega/globals.h>

#include <QtCore/QAbstractItemModel>

namespace Bodega {

    class Session;

    class BODEGA_EXPORT UpdatedAssetsModel : public QAbstractItemModel
    {
        Q_OBJECT
        Q_ENUMS(DisplayRoles)

    public:
        enum DisplayRoles {
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

            SessionRole = Qt::UserRole + 120
        };

        UpdatedAssetsModel(QObject *parent=0);
        ~UpdatedAssetsModel();

        Q_INVOKABLE void reload();

        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role) const;
        bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
        QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
        QModelIndex parent(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;

        void setSession(Session *session);
        Session *session() const;

    private:
        class Private;
        friend class Private;
        Private * const d;
        Q_PRIVATE_SLOT(d, void briefsJobFinished(Bodega::NetworkJob *))
        Q_PRIVATE_SLOT(d, void sessionAuthenticated(bool))
    };
}

#endif
