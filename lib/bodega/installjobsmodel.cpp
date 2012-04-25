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

#include "installjobsmodel.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QMetaEnum>

namespace Bodega
{

class InstallJobsModel::Private
{
public:
    Private(InstallJobsModel *parent);

    InstallJobsModel *q;
};

InstallJobsModel::Private::Private(InstallJobsModel *parent)
    : q(parent)
{
}


InstallJobsModel::InstallJobsModel(QObject *parent)
    : QStandardItemModel(parent),
      d(new Private(this))
{
    // set the role names based on the values of the DisplayRoles enum for
    //  the sake of QML
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "DisplayRole");
    roles.insert(Qt::DecorationRole, "DecorationRole");
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
}

InstallJobsModel::~InstallJobsModel()
{
    delete d;
}

void InstallJobsModel::addJob(const AssetInfo &info, InstallJob *job)
{
    QStandardItem *item = new QStandardItem(info.name);

    item->setData(info.images[ImageTiny], ImageTinyRole);
    item->setData(info.images[ImageSmall], ImageSmallRole);
    item->setData(info.images[ImageMedium], ImageMediumRole);
    item->setData(info.images[ImageLarge], ImageLargeRole);
    item->setData(info.images[ImageHuge], ImageHugeRole);
    item->setData(info.images[ImagePreviews], ImagePreviewsRole);

    item->setData(info.id, AssetIdRole);
    item->setData(0, ProgressRole);

    appendRow(item);
}

}

#include "installjobsmodel.moc"
