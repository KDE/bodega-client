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

#include "bodegamodel.h"

#include "session.h"
#include "channelsjob.h"

#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QMetaEnum>

using namespace Bodega;

static const int DEFAULT_PAGE_SIZE = 50;

class Node
{
public:
    Node(ChannelInfo channelInfo, AssetInfo assetInfo, Node *parent = 0);
    ~Node();

    void appendChild(Node *child);

    Node *child(int row);
    int childCount() const;
    int row() const;
    Node *parent();

    bool isChannel() const;
    const AssetInfo& assetInfo() const;
    const ChannelInfo& channelInfo() const;

    void setCompletelyPopulated(bool complete);
    bool isCompletelyPopulated() const;

    int lastFetchedOffset() const;
    void setLastFetchedOffset(int offset);

private:
    QList<Node*> m_childNodes;
    Node *m_parentNode;
    ChannelInfo m_channelInfo;
    AssetInfo m_assetInfo;
    bool m_completelyPopulated;
    int m_lastFetchedOffset;
};

Node::Node(ChannelInfo channelInfo, AssetInfo assetInfo, Node *parent)
    : m_parentNode(parent),
      m_assetInfo(assetInfo),
      m_channelInfo(channelInfo),
      m_completelyPopulated(false),
      m_lastFetchedOffset(0)
{
}

Node::~Node()
{
    qDeleteAll(m_childNodes);
}

void Node::appendChild(Node *item)
{
    m_childNodes.append(item);
}

Node *Node::child(int row)
{
    return m_childNodes.value(row);
}

int Node::childCount() const
{
    return m_childNodes.count();
}

Node *Node::parent()
{
    return m_parentNode;
}

int Node::row() const
{
    if (m_parentNode) {
        return m_parentNode->m_childNodes.indexOf(const_cast<Node*>(this));
    }

    return 0;
}

const AssetInfo &Node::assetInfo() const
{
    return m_assetInfo;
}

const ChannelInfo &Node::channelInfo() const
{
    return m_channelInfo;
}

bool Node::isChannel() const
{
    return !m_channelInfo.id.isEmpty();
}

void Node::setCompletelyPopulated(bool complete)
{
    m_completelyPopulated = complete;
}

bool Node::isCompletelyPopulated() const
{
    return m_completelyPopulated;
}

int Node::lastFetchedOffset() const
{
    return m_lastFetchedOffset;
}

void Node::setLastFetchedOffset(int offset)
{
    m_lastFetchedOffset = offset;
}




class Model::Private {
public:
    Private(Model *parent) :
        q(parent),
        session(0),
        topNode(0)
    {
        init();
    }
    void init();
    void channelsJobFinished(Bodega::NetworkJob *job);
    void reloadFromNetwork();

    Model *q;
    Session *session;
    QString topChannel;
    QString searchQuery;
    Node *topNode;

    //NEVER access those values outside channelsJobFinished(), Session will delete them
    //QPersistentModelIndex parent of this ChannelsJob 1/1 correspondence with Node
    QHash<ChannelsJob *, QPersistentModelIndex> indexForJobs;
    //job correspondednt to this  index
    QHash<QPersistentModelIndex, QList<ChannelsJob *> > jobsForIndex;
};

void Model::Private::init()
{
    ChannelInfo rootChanInfo;
    AssetInfo rootAssetInfo;
    delete topNode;
    topNode = new Node(rootChanInfo, rootAssetInfo, 0);
    indexForJobs.clear();
    jobsForIndex.clear();
    session = 0;
}

void Model::Private::channelsJobFinished(Bodega::NetworkJob *job)
{
    ChannelsJob *channelsJob = qobject_cast<ChannelsJob *>(job);
    Q_ASSERT(channelsJob);
    Q_ASSERT(indexForJobs.contains(channelsJob));


    QPersistentModelIndex idx = indexForJobs.value(channelsJob);

    //remove from bookkeeping
    indexForJobs.remove(channelsJob);
    jobsForIndex[idx].removeAll(channelsJob);
    if (jobsForIndex[idx].isEmpty()) {
        jobsForIndex.remove(idx);
    }

    if (channelsJob->channels().isEmpty() && channelsJob->assets().isEmpty()) {
        return;
    }

    Node *node;
    if (idx.isValid()) {
        node = static_cast<Node*>(idx.internalPointer());
    } else {
        node = topNode;
    }
    node->setCompletelyPopulated(!channelsJob->hasMoreAssets());
    node->setLastFetchedOffset(channelsJob->offset() + DEFAULT_PAGE_SIZE);

    if (!channelsJob->channels().isEmpty()) {
        q->beginInsertRows(idx, node->childCount(),
                           node->childCount() + channelsJob->channels().count()-1);
        AssetInfo dummyAssetInfo;
        foreach (const ChannelInfo &info, channelsJob->channels()) {
            node->appendChild(new Node(info, dummyAssetInfo, node));
        }
        q->endInsertRows();
    }

    if (!channelsJob->assets().isEmpty()) {
        q->beginInsertRows(idx, node->childCount(),
                           node->childCount() + channelsJob->assets().count()-1);
        ChannelInfo dummyChannelInfo;
        foreach (const AssetInfo &info, channelsJob->assets()) {
            node->appendChild(new Node(dummyChannelInfo, info, node));
        }
        q->endInsertRows();
    }
}

void Model::Private::reloadFromNetwork()
{
    if (!session || !session->isAuthenticated()) {
        return;
    }

    //FIXME: not safe, at this point most of jobs will be deleted by session
    foreach (const QList<ChannelsJob *> &list, jobsForIndex) {
        foreach (ChannelsJob *job, list) {
            disconnect(job, 0, q, 0);
        }
    }
    jobsForIndex.clear();
    indexForJobs.clear();

    //reset topnode, remove all the contents of the model
    q->beginResetModel();
    delete topNode;
    ChannelInfo rootChanInfo;
    AssetInfo rootAssetInfo;
    topNode = new Node(rootChanInfo, rootAssetInfo, 0);
    q->endResetModel();

    ChannelsJob *channelsJob;
    if (searchQuery.isEmpty()) {
        channelsJob = session->channels(topChannel, 0, DEFAULT_PAGE_SIZE);
    } else {
        channelsJob = session->search(searchQuery, topChannel,
                                      0, DEFAULT_PAGE_SIZE);
    }

    connect(channelsJob, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            q, SLOT(channelsJobFinished(Bodega::NetworkJob *)));
    //TODO: manage errors

    QPersistentModelIndex idx = QPersistentModelIndex(QModelIndex());
    indexForJobs[channelsJob] = idx;
    jobsForIndex[idx] << channelsJob;
}



Model::Model(QObject *parent)
    : QAbstractItemModel(parent),
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
}

Model::~Model()
{
    delete d;
}

bool Model::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return false;
    }

    Node *node = static_cast<Node*>(parent.internalPointer());
    return node->isChannel() && !node->isCompletelyPopulated() &&
            !d->jobsForIndex.contains(parent);
}

int Model::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    Node *node = static_cast<Node*>(index.internalPointer());

    if (node->isChannel()) {
        switch (role) {
        case Qt::DisplayRole:
            return node->channelInfo().name;
        case ChannelIdRole:
            return node->channelInfo().id;
        case ChannelNameRole:
            return node->channelInfo().name;
        case ChannelDescriptionRole:
            return node->channelInfo().description;
        case ChannelAssetCountRole:
            return node->channelInfo().assetCount;
        case ImageTinyRole:
            return node->channelInfo().images.value(Bodega::ImageTiny);
        case ImageSmallRole:
            return node->channelInfo().images.value(Bodega::ImageSmall);
        case ImageMediumRole:
            return node->channelInfo().images.value(Bodega::ImageMedium);
        case ImageLargeRole:
            return node->channelInfo().images.value(Bodega::ImageLarge);
        case ImageHugeRole:
            return node->channelInfo().images.value(Bodega::ImageHuge);
        case ImagePreviewsRole:
            return node->channelInfo().images.value(Bodega::ImagePreviews);
        default:
            return QVariant();
        }
    } else {
        switch (role) {
        case Qt::DisplayRole:
            return node->assetInfo().name;
        case AssetIdRole:
            return node->assetInfo().id;
        case AssetNameRole:
            return node->assetInfo().name;
        case AssetVersionRole:
            return node->assetInfo().version;
        case AssetFilenameRole:
            return node->assetInfo().filename;
        case AssetLicenseRole:
            return node->assetInfo().license;
        case AssetLicenseTextRole:
            return node->assetInfo().licenseText;
        case AssetPartnerIdRole:
            return node->assetInfo().partnerId;
        case AssetPartnerNameRole:
            return node->assetInfo().partnerName;
        case ImageTinyRole:
            return node->assetInfo().images.value(Bodega::ImageTiny);
        case ImageSmallRole:
            return node->assetInfo().images.value(Bodega::ImageSmall);
        case ImageMediumRole:
            return node->assetInfo().images.value(Bodega::ImageMedium);
        case ImageLargeRole:
            return node->assetInfo().images.value(Bodega::ImageLarge);
        case ImageHugeRole:
            return node->assetInfo().images.value(Bodega::ImageHuge);
        case ImagePreviewsRole:
            return node->assetInfo().images.value(Bodega::ImagePreviews);
        case AssetDescriptionRole:
            return node->assetInfo().description;
        case AssetPointsRole:
            return node->assetInfo().points;
        case AssetSizeRole:
            return node->assetInfo().size;
        default:
            return QVariant();
        }
    }
}

void Model::fetchMore(const QModelIndex &parent)
{
    if (!parent.isValid() || !d->session || !canFetchMore(parent) ||
        !d->session->isAuthenticated()) {
        return;
    }

    QPersistentModelIndex idx = QPersistentModelIndex(parent);
    Node *node = static_cast<Node*>(idx.internalPointer());

    ChannelsJob *channelsJob;
    if (d->searchQuery.isEmpty()) {
        channelsJob = d->session->channels(node->channelInfo().id,
                                           node->lastFetchedOffset(),
                                           DEFAULT_PAGE_SIZE);
    } else {
        channelsJob = d->session->search(d->searchQuery,
                                         d->topChannel,
                                         node->lastFetchedOffset(),
                                         DEFAULT_PAGE_SIZE);
    }
    connect(channelsJob, SIGNAL(jobFinished(Bodega::NetworkJob *)),
            this, SLOT(channelsJobFinished(Bodega::NetworkJob *)));
    //TODO: manage errors

    d->indexForJobs[channelsJob] = idx;
    d->jobsForIndex[idx] << channelsJob;
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::NoItemFlags;
    }
}

bool Model::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return true;
    }
    return static_cast<Node*>(parent.internalPointer())->isChannel();
}

QVariant Model::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    return QVariant();
}

QModelIndex Model::index(int row, int column,
                         const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    Node *parentNode;

    if (!parent.isValid()) {
        parentNode = d->topNode;
    } else {
        parentNode = static_cast<Node*>(parent.internalPointer());
    }

    Node *childNode = parentNode->child(row);
    if (childNode) {
        return createIndex(row, column, childNode);
    } else {
        return QModelIndex();
    }
}

QMap<int, QVariant> Model::itemData(const QModelIndex &index) const
{
    return QMap<int, QVariant>();
}

QModelIndex Model::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    Node *childNode = static_cast<Node*>(index.internalPointer());
    Node *parentNode = childNode->parent();

    if (parentNode == d->topNode) {
        return QModelIndex();
    }

    return createIndex(parentNode->row(), 0, parentNode);
}

int Model::rowCount(const QModelIndex &parent) const
{
    Node *parentNode;
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        parentNode = d->topNode;
    } else {
        parentNode = static_cast<Node*>(parent.internalPointer());
    }

    return parentNode->childCount();
}

void Model::setSession(Session *session)
{
    if (session == d->session) {
        return;
    }

    if (d->session) {
        disconnect(d->session, 0, this, 0);
        d->init();
        beginResetModel();
        endResetModel();
    }

    d->session = session;
    connect(d->session, SIGNAL(authenticated(bool)),
            this, SLOT(reloadFromNetwork()));

    d->reloadFromNetwork();
}

Session *Model::session() const
{
    return d->session;
}

void Model::setTopChannel(const QString &topChannel)
{
    if (d->topChannel == topChannel) {
        return;
    }

    d->topChannel = topChannel;

    if (!d->session) {
        return;
    }

    //Search queries win against channel listing
    if (!d->searchQuery.isEmpty()) {
        d->reloadFromNetwork();
    }
    emit topChannelChanged();
}

QString Model::topChannel() const
{
    return d->topChannel;
}

void Model::setSearchQuery(const QString &query)
{
    if (d->searchQuery == query) {
        return;
    }

    d->searchQuery = query;

    if (!d->session) {
        return;
    }

    d->reloadFromNetwork();
    emit searchQueryChanged();
}

QString Model::searchQuery() const
{
    return d->searchQuery;
}

#include "bodegamodel.moc"
