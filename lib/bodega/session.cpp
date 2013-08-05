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

#include "session.h"
#include "session_p.h"

#include "assetjob.h"
#include "assetoperations.h"
#include "collectionaddassetjob.h"
#include "collectionlistassetsjob.h"
#include "collectionremoveassetjob.h"
#include "channelsjob.h"
#include "changelanguagejob.h"
#include "createcollectionjob.h"
#include "deletecollectionjob.h"
#include "installjob.h"
#include "installjobsmodel.h"
#include "listcollectionsjob.h"
#include "participantinfojob.h"
#include "registerjob.h"
#include "signonjob.h"
#include "ratingattributesjob.h"
#include "assetratingsjob.h"
#include "participantratingsjob.h"

#include <QNetworkAccessManager>
#include <QDebug>
#include <QDir>

using namespace Bodega;

Session::Private::Private(Session *parent)
        : q(parent),
          points(0),
          authenticated(false),
          netManager(new QNetworkAccessManager(q)),
          installJobsModel(new InstallJobsModel(q))
{
}

void Session::Private::setPoints(int p)
{
    if (p > -1) {
        points = p;
        emit q->pointsChanged(points);
    }
}

void Session::Private::signOnFinished(SignOnJob *job)
{
    imageUrls = job->imageUrls();
}

void Session::Private::jobFinished(NetworkJob *job)
{
    job->deleteLater();

    if (!job->isJsonResponse()) {
        return;
    }

    if (authenticated && !job->authSuccess()) {
        setPoints(0);
        emit q->disconnected();
    }

    if (authenticated != job->authSuccess()) {
        authenticated = !authenticated;
        emit q->authenticated(authenticated);
    }

    if (job->authSuccess()) {
        setPoints(job->points());
    }
}

QNetworkReply *Session::Private::get(const QUrl &url)
{
    qDebug() << url;
    QNetworkRequest request;
    request.setRawHeader("User-Agent", "Bodega 0.1");
    request.setUrl(url);
    return netManager->get(request);
}

QNetworkReply *Session::Private::post(const QUrl &url, const QByteArray &data)
{
    qDebug() << url;
    QNetworkRequest request;
    request.setRawHeader("User-Agent", "Bodega 0.1");
    request.setUrl(url);
    return netManager->post(request, data);
}

void Session::Private::addPaging(QUrl &url, int offset, int pageSize)
{
    if (offset >= 0) {
        url.addQueryItem(QLatin1String("offset"), QString::number(offset));
    }

    if (pageSize >= 0) {
        url.addQueryItem(QLatin1String("pageSize"), QString::number(pageSize));
    }
}

Session::Session(QObject *parent)
    : QObject(parent),
      d(new Private(this))
{
}

Session::~Session()
{
    delete d;
}

QUrl Session::baseUrl() const
{
    return d->baseUrl;
}

void Session::setBaseUrl(const QUrl &url)
{
    d->baseUrl = url;
}

SignOnJob *Session::signOn()
{
    QUrl url = d->baseUrl;

    url.setEncodedPath(d->jsonPath("/auth"));
    url.addQueryItem(QLatin1String("auth_user"), d->userName);
    url.addQueryItem(QLatin1String("auth_password"), d->password);
    url.addQueryItem(QLatin1String("auth_device"), d->storeId);

    //qDebug()<<"url is " <<url;

    SignOnJob *job = new SignOnJob(d->get(url), this);
    connect(job, SIGNAL(signedOn(Bodega::SignOnJob*)), SLOT(signOnFinished(Bodega::SignOnJob*)));
    d->jobConnect(job);
    return job;
}

void Session::signOut()
{
    if (d->authenticated) {
        d->authenticated = false;
        d->setPoints(0);
        emit disconnected();
    }
}

QString Session::userName() const
{
    return d->userName;
}

void Session::setUserName(const QString &name)
{
    if (d->userName != name) {
        d->userName = name;
        emit userNameChanged();
    }
}

QString Session::password() const
{
    return d->password;
}

void Session::setPassword(const QString &password)
{
    if (d->password != password) {
        d->password = password;
        emit passwordChanged();
    }
}

QString Session::storeId() const
{
    return d->storeId;
}

void Bodega::Session::setStoreId(const QString &id)
{
    if (d->storeId != id) {
        d->storeId = id;
        emit storeIdChanged();
    }
}

bool Session::isAuthenticated() const
{
    return d->authenticated;
}

ChannelsJob * Session::channels(const QString &topChannel, int offset, int pageSize)
{
    QUrl url = d->baseUrl;
    QString path;

    if (topChannel.isEmpty()) {
        //list all channels
        path = QLatin1String("/channels");
    } else {
        path = QString::fromLatin1("/channel/%1").arg(topChannel);
    }

    url.setEncodedPath(d->jsonPath(path));
    d->addPaging(url, offset, pageSize);

    //qDebug()<<"url is " <<url;

    ChannelsJob *job = new ChannelsJob(topChannel, d->get(url), this);
    d->jobConnect(job);
    return job;
}

ChannelsJob * Session::search(const QString &text, const QString &channelId, int offset, int pageSize)
{
    QUrl url = d->baseUrl;
    const QString path = QLatin1String("/search");

    url.setEncodedPath(d->jsonPath(path));
    url.addQueryItem(QLatin1String("query"), text);

    //FIXME: decide how this should work, the search field may have to be moved in the channels column
    url.addQueryItem(QLatin1String("channelId"), channelId);
    d->addPaging(url, offset, pageSize);

    //qDebug()<<"url is " <<url;

    ChannelsJob *job = new ChannelsJob(text, d->get(url), this);
    d->jobConnect(job);
    return job;
}

ParticipantInfoJob *Session::participantInfo()
{
    QUrl url = d->baseUrl;
    const QString path = QLatin1String("/participant/info");
    url.setEncodedPath(d->jsonPath(path));

    ParticipantInfoJob *job = new ParticipantInfoJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

ChannelsJob * Session::nextChannels(const ChannelsJob *job)
{
    if (!job || !job->hasMoreAssets())
        return 0;

    int offset = job->offset() + 1;
    int pageSize = job->pageSize();

    return channels(job->channelId(),
                    offset, pageSize);
}

ChannelsJob * Session::prevChannels(const ChannelsJob *job)
{
    if (!job || job->offset() <= 0)
        return 0;

    int offset = job->offset() - 1;
    int pageSize = job->pageSize();

    return channels(job->channelId(),
                    offset, pageSize);
}

AssetJob * Session::asset(const QString &assetId,
                          AssetJob::AssetFlags flags)
{
    QUrl url = d->baseUrl;
    QString path = QString::fromLatin1("/asset/%1")
                   .arg(assetId);

    url.setEncodedPath(d->jsonPath(path));

    if (flags & AssetJob::ShowPreviews) {
        url.addQueryItem(QLatin1String("previews"),
                         QLatin1String("1"));
    }

    if (flags & AssetJob::ShowChangeLog) {
        url.addQueryItem(QLatin1String("changelog"),
                         QLatin1String("1"));
    }

    if (flags & AssetJob::Ratings) {
        url.addQueryItem(QLatin1String("ratings"),
                         QLatin1String("1"));
    }
    //qDebug()<<"url is " <<url;

    AssetJob *job = new AssetJob(assetId, d->get(url), this);
    d->jobConnect(job);
    return job;
}

ChangeLanguageJob * Session::changeLanguage(const QString &lang)
{
    QUrl url = d->baseUrl;
    QString path = QString::fromLatin1("/language/%1")
                   .arg(lang);

    url.setEncodedPath(d->jsonPath(path));

    //qDebug()<<"url is " <<url;

    ChangeLanguageJob *job = new ChangeLanguageJob(lang, d->get(url), this);

    d->jobConnect(job);

    return job;
}

int Session::points() const
{
    return d->points;
}

QMap<ImageUrl, QUrl> Session::urlsForImage(const QString &name) const
{
    QMap<ImageUrl, QUrl>::const_iterator itr;
    QMap<ImageUrl, QUrl> ret;
    for (itr = d->imageUrls.constBegin(); itr != d->imageUrls.constEnd();
         ++itr) {
        if (itr.key() == ImagePreviews) {
            // we don't care about the previews images, as there may be
            // multiple of them; they are fetched via AssetJob's previews()
            continue;
        }

        const QString path = QString::fromLatin1("%1/%2")
                             .arg(itr.value().toString())
                             .arg(name);

        ret.insert(itr.key(), QUrl(path));
    }
    return ret;
}

Bodega::InstallJobsModel *Session::installJobsModel() const
{
    return d->installJobsModel;
}

Bodega::NetworkJob *Session::history(int offset, int pageSize)
{
    QUrl url = d->baseUrl;
    const QString path = QLatin1String("/participant/history");
    url.setEncodedPath(d->jsonPath(path));
    d->addPaging(url, offset, pageSize);
    //qDebug() << "url is" << url;

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::AssetOperations *Session::assetOperations(const QString &assetId)
{
    return new AssetOperations(assetId, this);
}

Bodega::InstallJob *Session::install(AssetOperations *operations)
{
    QUrl url = d->baseUrl;
    const QString path = QLatin1String("/download/") + operations->assetInfo().id;
    url.setEncodedPath(d->jsonPath(path));

    //qDebug() << "url is" << url;

    QNetworkReply *reply = d->get(url);
    InstallJob *job = operations->install(reply, this);
    d->jobConnect(job);

    return job;
}

Bodega::UninstallJob *Session::uninstall(AssetOperations *operations)
{
    return operations->uninstall(this);
}

Bodega::NetworkJob *Session::redeemPointsCode(const QString &code)
{
    QUrl url = d->baseUrl;
    const QString path = QLatin1String("/points/redeemCode/") + code;
    url.setEncodedPath(d->jsonPath(path));
    //qDebug() << "url is" << url;

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::NetworkJob *Session::pointsPrice(int points)
{
    QUrl url = d->baseUrl;
    const QString path = QLatin1String("/points/price");
    url.setEncodedPath(d->jsonPath(path));

    if (points > 0) {
        url.addQueryItem(QLatin1String("amount"), QString::number(points));
    }

    qDebug() << "url is" << url;

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::NetworkJob *Session::buyPoints(int points)
{
    QUrl url = d->baseUrl;
    const QString path = QLatin1String("/points/buy");
    url.setEncodedPath(d->jsonPath(path));

    if (points > 0) {
        url.addQueryItem(QLatin1String("amount"), QString::number(points));
    }

    //qDebug() << "url is" << url;

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::NetworkJob *Session::purchaseAsset(const QString &assetId)
{
    QUrl url = d->baseUrl;
    const QString path = QLatin1String("/purchase/") + assetId;
    url.setEncodedPath(d->jsonPath(path));
    //qDebug() << "url is" << url;

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::NetworkJob *Session::paymentMethod()
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/participant/paymentMethod");
    url.setEncodedPath(d->jsonPath(path));
    //qDebug()<<"url is " <<url;

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::NetworkJob *Session::deletePaymentMethod()
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/participant/deletePaymentMethod");
    url.setEncodedPath(d->jsonPath(path));
    //qDebug()<<"url is " <<url;

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::NetworkJob *Session::setPaymentMethod(const QString &number, const QString &expiryMonth, const QString &expiryYear, const QString &cvc, const QString &name, const QString &address1, const QString &address2, const QString &addressZip, const QString &addressState, const QString &addressCountry)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/participant/changeAccountDetails");
    url.setEncodedPath(d->jsonPath(path));

    if (!number.isEmpty()) {
        url.addQueryItem(QLatin1String("card[number]"), number);
    }

    if (!expiryMonth.isEmpty()) {
        url.addQueryItem(QLatin1String("card[exp_month]"), expiryMonth);
    }

    if (!expiryYear.isEmpty()) {
        url.addQueryItem(QLatin1String("card[exp_year]"), expiryYear);
    }

    if (!cvc.isEmpty()) {
        url.addQueryItem(QLatin1String("card[cvc]"), cvc);
    }

    if (!name.isEmpty()) {
        url.addQueryItem(QLatin1String("card[name]"), name);
    }

    if (!address1.isEmpty()) {
        url.addQueryItem(QLatin1String("card[address_line1]"), address1);
    }

    if (!address2.isEmpty()) {
        url.addQueryItem(QLatin1String("card[address_line2]"), address2);
    }

    if (!addressZip.isEmpty()) {
        url.addQueryItem(QLatin1String("card[address_zip]"), addressZip);
    }

    if (!addressState.isEmpty()) {
        url.addQueryItem(QLatin1String("card[address_state]"), addressState);
    }

    if (!addressCountry.isEmpty()) {
        url.addQueryItem(QLatin1String("card[address_country]"), addressCountry);
    }

    //qDebug()<<"url is " <<url;

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}


Bodega::RegisterJob * Session::registerAccount(const QString &email,
                                               const QString &password,
                                               const QString &firstName,
                                               const QString &middleNames,
                                               const QString &lastName)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/register");

    url.setEncodedPath(d->jsonPath(path));

    url.addQueryItem(QLatin1String("email"),       email);
    url.addQueryItem(QLatin1String("password"),    password);
    url.addQueryItem(QLatin1String("firstname"),   firstName);
    url.addQueryItem(QLatin1String("middlenames"), middleNames);
    url.addQueryItem(QLatin1String("lastname"),    lastName);

    //qDebug()<< "url is" <<url;

    RegisterJob *job = new RegisterJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::NetworkJob *Session::resetPassword(const QString &email)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/participant/resetRequest");
    url.setEncodedPath(d->jsonPath(path));
    url.addQueryItem(QLatin1String("email"), email);

    //qDebug()<<"url is " <<url;

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::NetworkJob *Session::changePassword(const QString &newPassword)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/participant/changePassword");
    url.setEncodedPath(d->jsonPath(path));
    url.addQueryItem(QLatin1String("newPassword"), newPassword);

    //qDebug()<<"url is " <<url;

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::NetworkJob *Session::changeAccountDetails(const QString &firstName, const QString &lastName, const QString &email)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/participant/changeAccountDetails");
    url.setEncodedPath(d->jsonPath(path));

    if (!firstName.isEmpty()) {
        url.addQueryItem(QLatin1String("firstName"), firstName);
    }

    if (!lastName.isEmpty()) {
        url.addQueryItem(QLatin1String("lastName"), lastName);
    }

    if (!email.isEmpty()) {
        url.addQueryItem(QLatin1String("email"), email);
    }

    //qDebug()<<"url is " <<url;

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::ListCollectionsJob *Session::listcollections(int offset, int pageSize)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/collection/list");

    url.setEncodedPath(d->jsonPath(path));
    d->addPaging(url, offset, pageSize);

    //qDebug()<<"url is " <<url;

    ListCollectionsJob *job = new ListCollectionsJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::CreatecollectionJob * Session::createCollection(const QString &name,
                                                CollectionInfo::CollectionFlags flags)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/collections/create");

    url.setEncodedPath(d->jsonPath(path));

    url.addQueryItem(QLatin1String("name"), name);
    if (flags.testFlag(CollectionInfo::Public)) {
        url.addQueryItem(QLatin1String("public"), QLatin1String("true"));
    } else {
        url.addQueryItem(QLatin1String("public"), QLatin1String("false"));
    }
    if (flags.testFlag(CollectionInfo::Wishlist)) {
        url.addQueryItem(QLatin1String("wishlist"), QLatin1String("true"));
    } else {
        url.addQueryItem(QLatin1String("wishlist"), QLatin1String("false"));
    }

    //qDebug()<<"url is " <<url;

    CreatecollectionJob *job = new CreatecollectionJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::DeletecollectionJob * Session::deleteCollection(const QString &collectionId)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/collections/delete");

    url.setEncodedPath(d->jsonPath(path));

    url.addQueryItem(QLatin1String("collectionId"), collectionId);

    //qDebug()<<"url is " <<url;

    DeletecollectionJob *job = new DeletecollectionJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::collectionAddAssetJob * Session::collectionAddAsset(const QString &collectionId,
                                                    const QString &assetId)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/collections/addAsset");

    url.setEncodedPath(d->jsonPath(path));

    url.addQueryItem(QLatin1String("collectionId"), collectionId);
    url.addQueryItem(QLatin1String("assetId"), assetId);

    //qDebug()<<"url is " <<url;

    collectionAddAssetJob *job = new collectionAddAssetJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::collectionRemoveAssetJob * Session::collectionRemoveAsset(const QString &collectionId,
                                                          const QString &assetId)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/collections/removeAsset");

    url.setEncodedPath(d->jsonPath(path));

    url.addQueryItem(QLatin1String("collectionId"), collectionId);
    url.addQueryItem(QLatin1String("assetId"), assetId);

    //qDebug()<<"url is " <<url;

    collectionRemoveAssetJob *job = new collectionRemoveAssetJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::collectionListAssetsJob * Session::collectionListAssets(const QString &collectionId,
                                                        int offset,
                                                        int pageSize)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/collection/list/");
    url.setEncodedPath(d->jsonPath(path + collectionId));
    d->addPaging(url, offset, pageSize);

    //qDebug()<<"url is " <<url;

    collectionListAssetsJob *job = new collectionListAssetsJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::RatingAttributesJob * Session::listRatingAttributes(const QString &assetId)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/asset/ratings/attributes/%1").arg(assetId);

    url.setEncodedPath(d->jsonPath(path));

    RatingAttributesJob *job = new RatingAttributesJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::NetworkJob * Session::deleteAssetRatings(const QString &assetId)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/asset/ratings/delete/%1").arg(assetId);
    url.setEncodedPath(d->jsonPath(path));

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::AssetRatingsJob * Session::assetRatings(const QString &assetId)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/asset/ratings/list/%1").arg(assetId);
    url.setEncodedPath(d->jsonPath(path));

    AssetRatingsJob *job = new AssetRatingsJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::ParticipantRatingsJob * Session::participantRatings(int offset, int pageSize)
{
    QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/participant/ratings/");
    url.setEncodedPath(d->jsonPath(path));
    d->addPaging(url, offset, pageSize);

    ParticipantRatingsJob *job = new ParticipantRatingsJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::NetworkJob *Session::assetCreateRatings(const QString &assetId, const QVariantList &ratings)
{
   QUrl url = d->baseUrl;
    const QString path = QString::fromLatin1("/asset/ratings/create/%1").arg(assetId);
    url.setEncodedPath(d->jsonPath(path));

    NetworkJob *job = new NetworkJob(d->post(url, d->qvariantListToJson(ratings)), this);
    d->jobConnect(job);
    return job;
}

#include "session.moc"
