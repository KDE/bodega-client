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

#include <QtGui/QStandardItemModel>

namespace Bodega {

    class InstallJob;

    class BODEGA_EXPORT InstallJobsModel : public QStandardItemModel
    {
        Q_OBJECT
        Q_ENUMS(DisplayRoles)

    public:
        enum DisplayRoles {
            ProgressRole = Qt::UserRole + 1
        };

        InstallJobsModel(QObject *parent = 0);
        ~InstallJobsModel();

        void addJob(const AssetInfo &info, InstallJob *job);

    private:
        class Private;
        Private * const d;
    };

}

#endif
