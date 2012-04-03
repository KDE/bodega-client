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
#include "ballotaddassetjob.h"
#include "ballotlistassetsjob.h"
#include "ballotremoveassetjob.h"
#include "channelsjob.h"
#include "changelanguagejob.h"
#include "createballotjob.h"
#include "deleteballotjob.h"
#include "installjob.h"
#include "listballotsjob.h"
#include "participantinfojob.h"
#include "registerjob.h"
#include "resetpasswordjob.h"
#include "searchjob.h"
#include "signonjob.h"

#include <QNetworkAccessManager>
#include <QDebug>
#include <QDir>

using namespace Bodega;

Session::Private::Private(Session *parent)
        : q(parent),
          points(0),
          authenticated(false),
          netManager(new QNetworkAccessManager(q))
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

    job->deleteLater();
}

QNetworkReply *Session::Private::get(const QUrl &url)
{
    QNetworkRequest request;
    request.setRawHeader("User-Agent", "Bodega 0.1");
    request.setUrl(url);
    return netManager->get(request);
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
    url.addQueryItem(QLatin1String("auth_device"), d->deviceId);

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

QString Session::deviceId() const
{
    return d->deviceId;
}

void Bodega::Session::setDeviceId(const QString &device)
{
    if (d->deviceId != device) {
        d->deviceId = device;
        emit deviceChanged();
    }
}

bool Session::isAuthenticated() const
{
    return d->authenticated;
}

ChannelsJob * Session::channels(const QString &topChannel, int offset,
                                int pageSize)
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

    if (offset >= 0) {
        url.addQueryItem(QLatin1String("offset"),
                         QString::fromLatin1("%1").arg(offset));
    }
    if (pageSize >= 0) {
        url.addQueryItem(QLatin1String("pageSize"),
                         QString::fromLatin1("%1").arg(pageSize));
    }

    //qDebug()<<"url is " <<url;

    ChannelsJob *job = new ChannelsJob(topChannel, d->get(url), this);
    d->jobConnect(job);
    return job;
}

ChannelsJob * Session::search(const QString &text, const QString &channelId, int offset, int pageSize)
{
    QUrl url = d->baseUrl;
    QString path = QLatin1String("/search");

    url.setEncodedPath(d->jsonPath(path));
    url.addQueryItem(QLatin1String("query"), text);
    //FIXME: decide how this should work, the search field may have to be moved in the channels column
    url.addQueryItem(QLatin1String("channelId"), channelId);

    if (offset >= 0) {
        url.addQueryItem(QLatin1String("offset"),
                         QString::fromLatin1("%1").arg(offset));
    }
    if (pageSize >= 0) {
        url.addQueryItem(QLatin1String("pageSize"),
                         QString::fromLatin1("%1").arg(pageSize));
    }

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

    if (flags & AssetJob::ShowPreviews)
        url.addQueryItem(QLatin1String("previews"),
                         QLatin1String("1"));

    if (flags & AssetJob::ShowChangeLog)
        url.addQueryItem(QLatin1String("changelog"),
                         QLatin1String("1"));

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
        QString path = QString::fromLatin1("%1/%2")
                       .arg(itr.value().toString())
                       .arg(name);

        ret.insert(itr.key(), QUrl(path));
    }
    return ret;
}

Bodega::AssetOperations *Session::assetOperations(const QString &assetId)
{
    return new AssetOperations(assetId, this);
}

Bodega::InstallJob *Session::install(AssetOperations *operations)
{
    QNetworkReply *reply = d->get(operations->assetInfo().path);
    InstallJob *job = operations->install(reply, this);
    d->jobConnect(job);
    return job;
}

Bodega::UninstallJob *Session::uninstall(AssetOperations *operations)
{
    QNetworkReply *reply = d->get(operations->assetInfo().path);
    return operations->uninstall(this);
}

Bodega::NetworkJob *Session::redeemPointsCode(const QString &code)
{
    QUrl url = d->baseUrl;
    const QString path = QLatin1String("points/redeemCode/") + code;
    url.setEncodedPath(d->jsonPath(path));

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
    QString path = QString::fromLatin1("/register");

    url.setEncodedPath(d->jsonPath(path));

    url.addQueryItem(QLatin1String("email"),       email);
    url.addQueryItem(QLatin1String("password"),    password);
    url.addQueryItem(QLatin1String("firstname"),   firstName);
    url.addQueryItem(QLatin1String("middlenames"), middleNames);
    url.addQueryItem(QLatin1String("lastName"),    lastName);

    //qDebug()<<"url is " <<url;

    RegisterJob *job = new RegisterJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::ResetPasswordJob *Session::resetPassword(const QString &email)
{
    QUrl url = d->baseUrl;
    QString path = QString::fromLatin1("/resetRequest");
    url.setEncodedPath(d->jsonPath(path));
    url.addQueryItem(QLatin1String("email"), email);

    //qDebug()<<"url is " <<url;

    ResetPasswordJob *job = new ResetPasswordJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::NetworkJob *Session::changePassword(const QString &newPassword)
{
    QUrl url = d->baseUrl;
    QString path = QString::fromLatin1("/changePassword");
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
    QString path = QString::fromLatin1("/changeAccountDetails");
    url.setEncodedPath(d->jsonPath(path));

    if (!firstName.isEmpty()) {
        url.addQueryItem(QLatin1String("firstName"), firstName);
    }

    if (!lastName.isEmpty()) {
        url.addQueryItem(QLatin1String("lastName"), lastName);
    }

    if (!email.isEmpty()) {
        url.addQueryItem(QLatin1String("email"), firstName);
    }

    //qDebug()<<"url is " <<url;

    NetworkJob *job = new NetworkJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::ListBallotsJob *Session::listBallots(int offset, int pageSize)
{
    QUrl url = d->baseUrl;
    QString path = QString::fromLatin1("/listBallots");


    url.setEncodedPath(d->jsonPath(path));

    if (offset >= 0) {
        url.addQueryItem(QLatin1String("offset"),
                         QString::fromLatin1("%1").arg(offset));
    }
    if (pageSize >= 0) {
        url.addQueryItem(QLatin1String("pageSize"),
                         QString::fromLatin1("%1").arg(pageSize));
    }

    //qDebug()<<"url is " <<url;

    ListBallotsJob *job = new ListBallotsJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::CreateBallotJob * Session::createBallot(const QString &name,
                                                BallotInfo::BallotFlags flags)
{
    QUrl url = d->baseUrl;
    QString path = QString::fromLatin1("/createBallot");

    url.setEncodedPath(d->jsonPath(path));

    url.addQueryItem(QLatin1String("name"), name);
    if (flags.testFlag(BallotInfo::Public)) {
        url.addQueryItem(QLatin1String("public"), QLatin1String("true"));
    } else {
        url.addQueryItem(QLatin1String("public"), QLatin1String("false"));
    }
    if (flags.testFlag(BallotInfo::Wishlist)) {
        url.addQueryItem(QLatin1String("wishlist"), QLatin1String("true"));
    } else {
        url.addQueryItem(QLatin1String("wishlist"), QLatin1String("false"));
    }

    //qDebug()<<"url is " <<url;

    CreateBallotJob *job = new CreateBallotJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::DeleteBallotJob * Session::deleteBallot(const QString &ballotId)
{
    QUrl url = d->baseUrl;
    QString path = QString::fromLatin1("/deleteBallot");

    url.setEncodedPath(d->jsonPath(path));

    url.addQueryItem(QLatin1String("ballotId"), ballotId);

    //qDebug()<<"url is " <<url;

    DeleteBallotJob *job = new DeleteBallotJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::BallotAddAssetJob * Session::ballotAddAsset(const QString &ballotId,
                                                    const QString &assetId)
{
    QUrl url = d->baseUrl;
    QString path = QString::fromLatin1("/ballotAddAsset");

    url.setEncodedPath(d->jsonPath(path));

    url.addQueryItem(QLatin1String("ballotId"), ballotId);
    url.addQueryItem(QLatin1String("assetId"), assetId);

    //qDebug()<<"url is " <<url;

    BallotAddAssetJob *job = new BallotAddAssetJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::BallotRemoveAssetJob * Session::ballotRemoveAsset(const QString &ballotId,
                                                          const QString &assetId)
{
    QUrl url = d->baseUrl;
    QString path = QString::fromLatin1("/ballotRemoveAsset");

    url.setEncodedPath(d->jsonPath(path));

    url.addQueryItem(QLatin1String("ballotId"), ballotId);
    url.addQueryItem(QLatin1String("assetId"), assetId);

    //qDebug()<<"url is " <<url;

    BallotRemoveAssetJob *job = new BallotRemoveAssetJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

Bodega::BallotListAssetsJob * Session::ballotListAssets(const QString &ballotId,
                                                        int offset,
                                                        int pageSize)
{
    QUrl url = d->baseUrl;
    QString path = QString::fromLatin1("/ballotListAssets");


    url.setEncodedPath(d->jsonPath(path));

    if (offset >= 0) {
        url.addQueryItem(QLatin1String("offset"),
                         QString::number(offset));
    }
    if (pageSize >= 0) {
        url.addQueryItem(QLatin1String("pageSize"),
                         QString::number(pageSize));
    }

    //qDebug()<<"url is " <<url;

    BallotListAssetsJob *job = new BallotListAssetsJob(d->get(url), this);
    d->jobConnect(job);
    return job;
}

#include "session.moc"
