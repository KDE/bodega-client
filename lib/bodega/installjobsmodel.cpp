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

#include <bodega/installjob.h>

namespace Bodega
{


class InstallJobsModel::Private
{
public:
    Private(InstallJobsModel *parent);

    void progressChanged(qreal progress);
    void jobDestroyed(QObject *obj);

    InstallJobsModel *q;

    //list used as the model data
    QList <QHash<int, QVariant> > items;

    //Bookeeping for jobs
    //FIXME: This assumes rows will never be reordered
    QHash<InstallJob *, int > rowsForJobs;

    //the two hashes are intended to be perfectly symmetrical
    QHash<InstallJob *, QString> idsForJobs;
    QHash<QString, InstallJob *> jobsForIds;

    static InstallJobsModel* s_self;
};

InstallJobsModel* InstallJobsModel::Private::s_self = 0;

InstallJobsModel::Private::Private(InstallJobsModel *parent)
    : q(parent)
{
}

void InstallJobsModel::Private::progressChanged(qreal progress)
{
    InstallJob *job = qobject_cast<InstallJob *>(q->sender());

    if (!job || !rowsForJobs.contains(job)) {
        return;
    }

    const int row = rowsForJobs[job];
    items[row][ProgressRole] = progress;
    emit q->dataChanged(q->index(row, 0, QModelIndex()), q->index(row, 0, QModelIndex()));
}

void InstallJobsModel::Private::jobDestroyed(QObject *obj)
{
    //WARNING: this static_cast is safe because nobody will access the job* pointer
    // don't ever access it in this function!
    InstallJob *job = static_cast<InstallJob *>(obj);

    if (idsForJobs.contains(job)) {
        jobsForIds.remove(idsForJobs.value(job));
    }

    idsForJobs.remove(job);
    rowsForJobs.remove(job);
}

InstallJobsModel::InstallJobsModel(QObject *parent)
    : QAbstractListModel(parent),
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

InstallJobsModel* InstallJobsModel::self()
{
    if (!InstallJobsModel::Private::s_self) {
        InstallJobsModel::Private::s_self = new InstallJobsModel;
    }

    return InstallJobsModel::Private::s_self;
}

Bodega::InstallJob *InstallJobsModel::jobForAsset(const QString &assetId) const
{
    return d->jobsForIds.value(assetId);
}

void InstallJobsModel::addJob(const AssetInfo &info, InstallJob *job)
{
    QHash<int, QVariant> item;
    item[Qt::DisplayRole] = info.name;
    item[ImageTinyRole] = info.images[ImageTiny];
    item[ImageSmallRole] = info.images[ImageSmall];
    item[ImageMediumRole] = info.images[ImageMedium];
    item[ImageLargeRole] = info.images[ImageLarge];
    item[ImageHugeRole] = info.images[ImageHuge];
    item[ImagePreviewsRole] = info.images[ImagePreviews];

    item[AssetIdRole] = info.id;
    item[ProgressRole] = job->progress();

    d->rowsForJobs[job] = d->items.count();
    d->jobsForIds[info.id] = job;
    d->idsForJobs[job] = info.id;

    beginInsertRows(QModelIndex(), d->items.count(), d->items.count());
    d->items << item;
    endInsertRows();

    connect(job, SIGNAL(progressChanged(qreal)), this, SLOT(progressChanged(qreal)));
    connect(job, SIGNAL(destroyed(QObject *)), this, SLOT(jobDestroyed(QObject *)));
}


QVariant InstallJobsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= d->items.count() || index.row() < 0) {
        return QVariant();
    }

    return d->items[index.row()].value(role);
}

Qt::ItemFlags InstallJobsModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

int InstallJobsModel::rowCount(const QModelIndex &parent) const
{
    return d->items.size();
}

}

#include "installjobsmodel.moc"
