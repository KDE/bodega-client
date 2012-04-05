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

#include "historymodel.h"

#include "session.h"
#include "networkjob.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QMetaEnum>

namespace Bodega
{

static const int DEFAULT_PAGE_SIZE = 50;


class HistoryModel::Private {
public:
    Private(HistoryModel *parent);
    void historyJobFinished(Bodega::NetworkJob *job);
    void reloadFromNetwork();

    HistoryModel *q;
    Session *session;
    QList<QVariantMap> history;
    bool hasMore;
};

HistoryModel::Private::Private(HistoryModel *parent)
    : q(parent),
      session(0),
      hasMore(false)
{
}

void HistoryModel::Private::historyJobFinished(Bodega::NetworkJob *job)
{
    if (job->failed()) {
        return;
    }

    const QVariantMap json = job->parsedJson();
    if (json.isEmpty() || !json.contains(QLatin1String("history"))) {
        return;
    }

    hasMore = json[QLatin1String("hasMoreHistory")].toBool();
    const QVariantList historyData = json[QLatin1String("history")].toList();
    const int count = historyData.count();
    int offset = json[QLatin1String("offset")].toInt();
    const int end = count + offset - 1;
    q->beginInsertRows(QModelIndex(), offset, end);

    // fill any blanks
    while (history.size() < offset) {
        history.append(QVariantMap());
    }

    // insert the actual values we retrieved with this job
    for (QVariantList::const_iterator itr = historyData.constBegin(); itr != historyData.constEnd(); ++itr) {
        history.insert(offset, itr->toMap());
        ++offset;
    }

    q->endInsertRows();
}

void HistoryModel::Private::reloadFromNetwork()
{
    if (!session || !session->isAuthenticated()) {
        return;
    }

    //reset topnode, remove all the contents of the model
    q->beginResetModel();
    hasMore = false;
    history.clear();
    q->endResetModel();

    NetworkJob *historyJob = session->history(0, DEFAULT_PAGE_SIZE);
    connect(historyJob, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(historyJobFinished(Bodega::NetworkJob *)));
}

HistoryModel::HistoryModel(QObject *parent)
    : QAbstractItemModel(parent),
      d(new Private(this))
{
    // set the role names based on the values of the DisplayRoles enum for
    //  the sake of QML
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "DisplayRole");
    QMetaEnum e = metaObject()->enumerator(metaObject()->indexOfEnumerator("DisplayRoles"));
    for (int i = 0; i < e.keyCount(); ++i) {
        roles.insert(e.value(i), e.key(i));
    }
    setRoleNames(roles);
}

HistoryModel::~HistoryModel()
{
    delete d;
}

bool HistoryModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return false;
    }

    return d->hasMore;
}

int HistoryModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= d->history.count()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole: {
            const QString category = d->history[index.row()][QLatin1String("category")].toString();
            const QString what = d->history[index.row()][QLatin1String("what")].toString();
            if (category == QLatin1String("Download")) {
                return tr("Downloaded %1").arg(what);
            } else if (category == QLatin1String("Purchase")) {
                return tr("Purchased %1").arg(what);
            } else if (category == QLatin1String("Points")) {
                return tr("Added %1 points").arg(what);
            } else {
                return QString();
            }
        }
        case DateRole:
            return d->history[index.row()][QLatin1String("date")];
        case DescriptionRole:
            return d->history[index.row()][QLatin1String("comment")];
        default:
            return QVariant();
    }
}

void HistoryModel::fetchMore(const QModelIndex &parent)
{
    if (!parent.isValid() || !d->session || !canFetchMore(parent) ||
        !d->session->isAuthenticated()) {
        return;
    }

    NetworkJob *job = d->session->history(d->history.count(), DEFAULT_PAGE_SIZE);
    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            this, SLOT(historyJobFinished(Bodega::NetworkJob *)));
}

Qt::ItemFlags HistoryModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool HistoryModel::hasChildren(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return false;
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    return QVariant();
}

QModelIndex HistoryModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column > 0) {
        return QModelIndex();
    }

    if (row < 0 || row >= d->history.count()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QMap<int, QVariant> HistoryModel::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex HistoryModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int HistoryModel::rowCount(const QModelIndex &parent) const
{
    return d->history.size();
}

void HistoryModel::setSession(Session *session)
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
            this, SLOT(reloadFromNetwork()));

    if (d->session->isAuthenticated()) {
        d->reloadFromNetwork();
    }
}

Session *HistoryModel::session() const
{
    return d->session;
}

}

#include "historymodel.moc"
