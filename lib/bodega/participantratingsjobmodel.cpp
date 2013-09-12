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

#include "participantratingsjobmodel.h"

#include "participantratingsjob.h"
#include "assetjob.h"
#include "ratingattributesjob.h"
#include "session.h"
#include "networkjob.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QMetaEnum>
#include <QDebug>

namespace Bodega
{

struct AssetRatings
{
    QString assetId;
    AssetInfo assetInfo;
    QList<ParticipantRatings> participantRatings;
    inline bool operator==(const AssetRatings &asset)
    {
        return asset.assetId == this->assetId;
    };
};

class ParticipantRatingsJobModel::Private {
public:
    Private(ParticipantRatingsJobModel *parent);

    ParticipantRatingsJobModel *q;
    Session *session;
    void fetchParticipantRatings();
    void participantRatingsJobFinished(Bodega::NetworkJob *job);
    void assetJobFinished(Bodega::NetworkJob *job);
    void ratingAttributesJobFinished(Bodega::NetworkJob *job);

    QString findAttributeName(const QString &attributeId) const;
    QVariantList dataToList(int index) const;

    QList<RatingAttributes> ratingAttributes;
    QList<AssetRatings> dataList;
    int jobCounter;
};

ParticipantRatingsJobModel::Private::Private(ParticipantRatingsJobModel *parent)
    : q(parent),
      session(0),
      jobCounter(-1)
{
}

void ParticipantRatingsJobModel::Private::fetchParticipantRatings()
{
    ParticipantRatingsJob *job = session->participantRatings();

    q->beginResetModel();
    ratingAttributes.clear();
    dataList.clear();
    jobCounter = -1;
    q->endResetModel();

    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(participantRatingsJobFinished(Bodega::NetworkJob *)));
}

void ParticipantRatingsJobModel::Private::participantRatingsJobFinished(Bodega::NetworkJob *job)
{
    ParticipantRatingsJob *participantRatingsJob = qobject_cast<ParticipantRatingsJob*>(job);

    if (!participantRatingsJob) {
        return;
    }

    participantRatingsJob->deleteLater();

    if (participantRatingsJob->failed()) {
        return;
    }

    QList<ParticipantRatings> participantRatings = participantRatingsJob->ratings();
    jobCounter = participantRatings.count() * 2;

    foreach (const ParticipantRatings &r, participantRatings) {
        AssetRatings tmp;
        tmp.assetId = r.assetId;

        if (!dataList.contains(tmp)) {
            dataList << tmp;
        }

        const int index = dataList.indexOf(tmp);
        tmp = dataList.takeAt(index);
        tmp.participantRatings.append(r);
        dataList << tmp;

        AssetJob *assetJob = session->asset(r.assetId, AssetJob::ShowRatings);
        connect(assetJob, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(assetJobFinished(Bodega::NetworkJob *)));

        RatingAttributesJob *ratingAttributesJob = session->listRatingAttributes(r.assetId);
        connect(ratingAttributesJob, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(ratingAttributesJobFinished(Bodega::NetworkJob *)));
    }

    const int begin = 0;
    const int end = qMax(begin, dataList.count() + begin -1);

    q->beginInsertRows(QModelIndex(), begin, end);
}

void ParticipantRatingsJobModel::Private::assetJobFinished(Bodega::NetworkJob *job)
{
    AssetJob *assetJob = qobject_cast<AssetJob*>(job);

    if (assetJob) {
        assetJob->deleteLater();
        if (!assetJob->failed()) {
            AssetRatings tmp;
            tmp.assetId = assetJob->info().id;
            const int index = dataList.indexOf(tmp);
            tmp = dataList.takeAt(index);
            tmp.assetInfo = assetJob->info();
            dataList << tmp;
        }
    }

    jobCounter--;
    if (jobCounter == 0) {
        q->endInsertRows();
    }
}

void ParticipantRatingsJobModel::Private::ratingAttributesJobFinished(Bodega::NetworkJob *job)
{
    RatingAttributesJob *ratingAttributesJob = qobject_cast<RatingAttributesJob*>(job);

    if (ratingAttributesJob) {
        ratingAttributesJob->deleteLater();
        if (!ratingAttributesJob->failed()) {
            ratingAttributes.append(ratingAttributesJob->ratingAttributes());
        }
    }

    jobCounter--;
    if (jobCounter == 0) {
        q->endInsertRows();
    }

}

QString ParticipantRatingsJobModel::Private::findAttributeName(const QString &attributeId) const
{
    foreach(const RatingAttributes &attribute, ratingAttributes) {
        if (attribute.id == attributeId) {
            return attribute.name;
        }
    }
    return QString();
}

QVariantList ParticipantRatingsJobModel::Private::dataToList(int index) const
{
    QVariantList l;
    foreach(const ParticipantRatings &r, dataList.at(index).participantRatings) {
        QVariantMap data;
        data.insert(QLatin1String("AttributeName"), findAttributeName(r.attributeId));
        data.insert(QLatin1String("Rating"), r.rating);
        l.append(data);
    }
    return l;
}



ParticipantRatingsJobModel::ParticipantRatingsJobModel(QObject *parent)
    : QAbstractItemModel(parent),
      d(new Private(this))
{
    // set the role names based on the values of the DisplayRoles enum for
    //  the sake of QML
    QHash<int, QByteArray> roles;

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
    connect(this, SIGNAL(reload()),
            this, SLOT(fetchParticipantRatings()));
}

ParticipantRatingsJobModel::~ParticipantRatingsJobModel()
{
    delete d;
}

int ParticipantRatingsJobModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant ParticipantRatingsJobModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= d->dataList.count()) {
        return QVariant();
    }

    switch (role) {
        case AssetName: {
            return d->dataList.at(index.row()).assetInfo.name;
        }
        case AssetVersion: {
            return d->dataList.at(index.row()).assetInfo.version;
        }
        case AssetDesciption: {
            return d->dataList.at(index.row()).assetInfo.description;
        }
        case AssetId: {
            return d->dataList.at(index.row()).assetInfo.id;
        }
        case Ratings: {
            return d->dataToList(index.row());
        }
        default: {
            return QVariant();
        }
    }
}

Qt::ItemFlags ParticipantRatingsJobModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool ParticipantRatingsJobModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return false;
}

QVariant ParticipantRatingsJobModel::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    return QVariant();
}

QModelIndex ParticipantRatingsJobModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0) {
        return QModelIndex();
    }

    if (row < 0 || row >= d->dataList.count()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QMap<int, QVariant> ParticipantRatingsJobModel::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex ParticipantRatingsJobModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int ParticipantRatingsJobModel::rowCount(const QModelIndex &parent) const
{
    return d->dataList.count();
}

void ParticipantRatingsJobModel::setSession(Session *session)
{
    if (session == d->session) {
        return;
    }

    if (d->session) {
        //not connected directly, so disconnect everything
        d->session->disconnect(this);
    }
    d->session = session;

    if (!d->session) {
        return;
    }
    connect(d->session, SIGNAL(authenticated(bool)),
            this, SLOT(fetchParticipantRatings()));
}

Session *ParticipantRatingsJobModel::session() const
{
    return d->session;
}

}

#include "participantratingsjobmodel.moc"
