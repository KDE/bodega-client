#include "createballotjob.h"

using namespace Bodega;

class CreateBallotJob::Private
{
public:
    Private()
    {}

    void init(CreateBallotJob *q, const QUrl &url);
    void parseBallots(const QVariantMap &result);
    CreateBallotJob *q;
    BallotInfo ballot;
};

void CreateBallotJob::Private::init(CreateBallotJob *parent,
                                   const QUrl &url)
{
    q = parent;
}

void CreateBallotJob::Private::parseBallots(const QVariantMap &result)
{
    QVariantList ballotsLst = result[QLatin1String("ballots")].toList();

    Q_ASSERT(ballotsLst.count() == 1);

    QVariantMap ballot = ballotsLst[0].toMap();
    BallotInfo info;
    info.id = ballot[QLatin1String("id")].toInt();
    info.name = ballot[QLatin1String("name")].toString();
    info.flags = BallotInfo::None;
    if (ballot[QLatin1String("public")].toBool()) {
        info.flags |= BallotInfo::Public;
    }
    if (ballot[QLatin1String("wishlist")].toBool()) {
        info.flags |= BallotInfo::Wishlist;
    }

    this->ballot = info;
}

CreateBallotJob::CreateBallotJob(QNetworkReply *reply,
                                 Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

CreateBallotJob::~CreateBallotJob()
{
    delete d;
}

BallotInfo CreateBallotJob::ballot() const
{
    return d->ballot;
}

void CreateBallotJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);

    if (authSuccess() && !failed()) {
        d->parseBallots(result);
    }
}

#include "createballotjob.moc"
