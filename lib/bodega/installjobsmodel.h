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

#ifndef BODEGA_INSTALLJOBSMODEL_H
#define BODEGA_INSTALLJOBSMODEL_H

#include <bodega/globals.h>

#include <QtCore/QAbstractListModel>

namespace Bodega {

    class InstallJob;

    class BODEGA_EXPORT InstallJobsModel : public QAbstractListModel
    {
        Q_OBJECT
        Q_ENUMS(DisplayRoles)
        Q_PROPERTY(int count READ count NOTIFY countChanged)

    public:
        enum DisplayRoles {
            AssetIdRole = Qt::UserRole + 1,
            ProgressRole = Qt::UserRole + 2,

            ImageTinyRole = Qt::UserRole + 50,
            ImageSmallRole = Qt::UserRole + 51,
            ImageMediumRole = Qt::UserRole + 52,
            ImageLargeRole = Qt::UserRole + 53,
            ImageHugeRole = Qt::UserRole + 54,
            ImagePreviewsRole = Qt::UserRole + 55,
        };

        ~InstallJobsModel();

        static InstallJobsModel* self();

        Q_INVOKABLE Bodega::InstallJob *jobForAsset(const QString &assetId) const;

        //TODO: private?
        void addJob(const AssetInfo &info, InstallJob *job);

        int count() const {return rowCount();}

        //Reimplemented from QAbstractItemModel
        QVariant data(const QModelIndex &index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;

    Q_SIGNALS:
        void countChanged();
        void jobAdded(const Bodega::AssetInfo &info, Bodega::InstallJob *job);

    protected:
        InstallJobsModel(QObject *parent=0);

    private:
        class Private;
        Private * const d;
        Q_PRIVATE_SLOT(d, void progressChanged(qreal progress))
        Q_PRIVATE_SLOT(d, void jobDestroyed(QObject *obj))
    };

}

#endif
