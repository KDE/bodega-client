#include "listballotsjob.h"

using namespace Bodega;

class ListBallotsJob::Private
{
public:
    Private()
        : hasMoreBallots(false)
    {}

    void init(ListBallotsJob *q, const QUrl &url);
    void parseBallots(const QVariantMap &result);
    ListBallotsJob *q;
    QList<BallotInfo> ballots;
    bool hasMoreBallots;
    int offset;
    int pageSize;
};

void ListBallotsJob::Private::init(ListBallotsJob *parent,
                                   const QUrl &url)
{
    q = parent;
    QString offsetStr = url.queryItemValue(
        QLatin1String("offset"));
    QString pageSizeStr = url.queryItemValue(
        QLatin1String("pageSize"));

    if (offsetStr.isEmpty())
        offset = -1;
    else
        offset = offsetStr.toInt();

    if (pageSizeStr.isEmpty())
        pageSize = -1;
    else
        pageSize = pageSizeStr.toInt();
}

void ListBallotsJob::Private::parseBallots(const QVariantMap &result)
{
    QVariantList ballotsLst = result[QLatin1String("ballots")].toList();
    QVariantList::const_iterator itr;
    for (itr = ballotsLst.constBegin(); itr != ballotsLst.constEnd(); ++itr) {
        BallotInfo info;
        QVariantMap ballot = itr->toMap();
        info.id = ballot[QLatin1String("id")].toInt();
        info.name = ballot[QLatin1String("name")].toString();
        info.flags = BallotInfo::None;
        if (ballot[QLatin1String("public")].toBool()) {
            info.flags |= BallotInfo::Public;
        }
        if (ballot[QLatin1String("wishlist")].toBool()) {
            info.flags |= BallotInfo::Wishlist;
        }
        ballots.append(info);
    }
}

ListBallotsJob::ListBallotsJob(QNetworkReply *reply,
                               Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

ListBallotsJob::~ListBallotsJob()
{
    delete d;
}

QList<BallotInfo> ListBallotsJob::ballots() const
{
    return d->ballots;
}

bool ListBallotsJob::hasMoreBallots() const
{
    return d->hasMoreBallots;
}

int ListBallotsJob::offset() const
{
    return d->offset;
}

int ListBallotsJob::pageSize() const
{
    return d->pageSize;
}

void ListBallotsJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parseBallots(result);
        d->hasMoreBallots = result[QLatin1String("hasMoreBallots")].toBool();
    }
}

#include "listballotsjob.moc"