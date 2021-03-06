/***************************************************************************
 *                                                                         *
 *   Copyright 2012 Marco Martin <mart@kde.org>                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "bodegastore.h"
#include "kdeclarativeview.h"

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QDBusServiceWatcher>
#include <QDBusConnection>

#include <KConfig>
#include <KConfigGroup>
#include <KDebug>
#include <KGlobal>
#include <KCmdLineArgs>
#include <kwallet.h>

//Bodega libs
#include <bodega/assetoperations.h>
#include <bodega/bodegamodel.h>
#include <bodega/channelsjob.h>
#include <bodega/historymodel.h>
#include <bodega/networkjob.h>
#include <bodega/participantinfojob.h>
#include <bodega/registerjob.h>
#include <bodega/session.h>
#include <bodega/signonjob.h>
#include <bodega/installjob.h>
#include <bodega/installjobsmodel.h>
#include <bodega/installjobscheduler.h>
#include <bodega/uninstalljob.h>
#include <bodega/collectionsmodel.h>
#include <bodega/collectionassetsmodel.h>
#include <bodega/participantratingsmodel.h>
#include <bodega/participantratingsjob.h>
#include <bodega/assetratingsjob.h>
#include <bodega/assetratingsmodel.h>
#include <bodega/updatedassetsmodel.h>

using namespace Bodega;

static const QString s_bodegaUpdaterServiceName("org.kde.BodegaUpdater");

QScriptValue qScriptValueFromStatus(QScriptEngine *, const Bodega::InstallJobScheduler::InstallStatus &status)
{
    return QScriptValue((int)status);
}

void statusFromQScriptValue(const QScriptValue &scriptValue, Bodega::InstallJobScheduler::InstallStatus &status)
{
    status = (Bodega::InstallJobScheduler::InstallStatus)scriptValue.toInteger();
}

QScriptValue qScriptValueFromError(QScriptEngine *engine, const Bodega::Error &error)
{
    QScriptValue obj = engine->newObject();

    obj.setProperty("serverCode", error.serverCode());
    obj.setProperty("type", error.type());
    obj.setProperty("errorId", error.errorId());
    obj.setProperty("title", error.title());
    obj.setProperty("description", error.description());

    return obj;
}

void errorFromQScriptValue(const QScriptValue &scriptValue, Bodega::Error &error)
{
    Error::Type type = Error::Network;
    QString errorId;
    QString title;
    QString description;

    QScriptValueIterator it(scriptValue);

    while (it.hasNext()) {
        it.next();
        //kDebug() << it.name() << "is" << it.value().toString();
        if (it.name() == "serverCode") {
            const ErrorCodes::ServerCode code = (ErrorCodes::ServerCode)it.value().toInteger();
            if (code != ErrorCodes::NoCode) {
                error = Error(code);
                return;
            }
        } else if (it.name() == "type") {
            type = (Error::Type)it.value().toInteger();
        } else if (it.name() == "errorId") {
            errorId = it.value().toString();
        } else if (it.name() == "title") {
            title = it.value().toString();
        } else if (it.name() == "description") {
            description = it.value().toString();
        }
    }

    error = Error(type, errorId, title, description);
}


QScriptValue qScriptValueFromChannelInfo(QScriptEngine *engine, const Bodega::ChannelInfo &info)
{
    QScriptValue obj = engine->newObject();

    obj.setProperty("id", info.id);
    obj.setProperty("name", info.name);
    obj.setProperty("description", info.description);
    obj.setProperty("assetCount", info.assetCount);
    //obj.setProperty("images", images);

    return obj;
}

void channelInfoFromQScriptValue(const QScriptValue &scriptValue, Bodega::ChannelInfo &info)
{
    QScriptValueIterator it(scriptValue);

    while (it.hasNext()) {
        it.next();
        //kDebug() << it.name() << "is" << it.value().toString();
        if (it.name() == "id") {
            info.id = it.value().toString();
        } else if (it.name() == "name") {
            info.name = it.value().toString();
        } else if (it.name() == "description") {
            info.description = it.value().toString();
        } else if (it.name() == "assetCount") {
            info.assetCount = it.value().toInteger();
        }
    }
}

QScriptValue qScriptValueFromAssetInfo(QScriptEngine *engine, const Bodega::AssetInfo &info)
{
    QScriptValue obj = engine->newObject();

    obj.setProperty("id", info.id);
    obj.setProperty("license", info.license);
    obj.setProperty("partnerId", info.partnerId);
    obj.setProperty("partnerName", info.partnerName);
    obj.setProperty("name", info.name);
    obj.setProperty("filename", info.filename);
    obj.setProperty("version", info.version);
    //obj.setProperty("images", info.images);
    obj.setProperty("description", info.description);
    obj.setProperty("points", info.points);
    obj.setProperty("canDownload", info.canDownload);
    obj.setProperty("formatedSize", info.formatedSize);

    QScriptValue imageObj = engine->newObject();
    imageObj.setProperty("tiny", info.images[ImageTiny].toString());
    imageObj.setProperty("small", info.images[ImageSmall].toString());
    imageObj.setProperty("medium", info.images[ImageMedium].toString());
    imageObj.setProperty("big", info.images[ImageBig].toString());
    imageObj.setProperty("large", info.images[ImageLarge].toString());
    imageObj.setProperty("huge", info.images[ImageHuge].toString());
    imageObj.setProperty("previews", info.images[ImagePreviews].toString());
    obj.setProperty("images", imageObj);

    QScriptValue previewsObj = engine->newObject();
    if (info.previews.contains(ScreenShot1)) {
        previewsObj.setProperty("screenshot1", info.previews[ScreenShot1].toString());
    }
    if (info.previews.contains(ScreenShot2)) {
        previewsObj.setProperty("screenshot2", info.previews[ScreenShot2].toString());
    }
    if (info.previews.contains(CoverFront)) {
        previewsObj.setProperty("coverfront", info.previews[CoverFront].toString());
    }
    if (info.previews.contains(CoverBack)) {
        previewsObj.setProperty("coverback", info.previews[CoverBack].toString());
    }
    obj.setProperty("previews", previewsObj);

    return obj;
}

void assetInfoFromQScriptValue(const QScriptValue &scriptValue, Bodega::AssetInfo &info)
{
    QScriptValueIterator it(scriptValue);

    while (it.hasNext()) {
        it.next();
        if (it.name() == "id") {
            info.id = it.value().toString();
        } else if (it.name() == "license") {
            info.license = it.value().toString();
        } else if (it.name() == "partnerId") {
            info.partnerId = it.value().toString();
        } else if (it.name() == "partnerName") {
            info.partnerName = it.value().toString();
        } else if (it.name() == "name") {
            info.name = it.value().toString();
        } else if (it.name() == "filename") {
            info.filename = it.value().toString();
        } else if (it.name() == "version") {
            info.version = it.value().toString();
        } else if (it.name() == "description") {
            info.description = it.value().toString();
        } else if (it.name() == "points") {
            info.points = it.value().toInteger();
        } else if (it.name() == "canDownload") {
            info.canDownload = it.value().toBool();
        } else if (it.name() == "images") {
            QMap<ImageUrl, QUrl> images;
            QScriptValueIterator imageIt(scriptValue);

            while (imageIt.hasNext()) {
                imageIt.next();
                if (imageIt.name() == "tiny") {
                    images[ImageTiny] = imageIt.value().toString();
                } else if (imageIt.name() == "small") {
                    images[ImageSmall] = imageIt.value().toString();
                } else if (imageIt.name() == "medium") {
                    images[ImageMedium] = imageIt.value().toString();
                } else if (imageIt.name() == "big") {
                    images[ImageBig] = imageIt.value().toString();
                } else if (imageIt.name() == "large") {
                    images[ImageLarge] = imageIt.value().toString();
                } else if (imageIt.name() == "huge") {
                    images[ImageHuge] = imageIt.value().toString();
                } else if (imageIt.name() == "previews") {
                    images[ImagePreviews] = imageIt.value().toString();
                }
            }
            info.images = images;
        } else if (it.name() == "previews") {
            QMap<PreviewType, QUrl> previews;
            QScriptValueIterator previewIt(scriptValue);

            while (previewIt.hasNext()) {
                previewIt.next();
                if (previewIt.name() == "screenshot1") {
                    previews[ScreenShot1] = previewIt.value().toString();
                } else if (previewIt.name() == "screenshot2") {
                    previews[ScreenShot2] = previewIt.value().toString();
                } else if (previewIt.name() == "coverfront") {
                    previews[CoverFront] = previewIt.value().toString();
                } else if (previewIt.name() == "coverback") {
                    previews[CoverBack] = previewIt.value().toString();
                }
            }
            info.previews = previews;
        }
    }
}

QScriptValue qScriptValueFromTags(QScriptEngine *engine, const Bodega::Tags &tags)
{
    QScriptValue obj = engine->newObject();

    foreach (const QString &key, tags.keys()) {
        QScriptValue list = engine->newArray();
        int i = 0;
        foreach (const QString &value, tags.values(key)) {
            list.setProperty(i, value);
            ++i;
        }
        obj.setProperty(key, list);
    }

    return obj;
}

void tagsFromQScriptValue(const QScriptValue &scriptValue, Bodega::Tags &tags)
{
    QScriptValueIterator it(scriptValue);

    while (it.hasNext()) {
        it.next();
        QScriptValueIterator tagIterator(it.value());
        while (tagIterator.hasNext()) {
            tags.insert(it.name(), tagIterator.value().toString());
        }
    }
}

QScriptValue qScriptValueFromParticipantInfo(QScriptEngine *engine, const Bodega::ParticipantInfo &info)
{
    QScriptValue obj = engine->newObject();
    obj.setProperty("assetCount", info.assetCount);
    obj.setProperty("channelCount", info.channelCount);
    obj.setProperty("pointsOwed", info.pointsOwed);
    obj.setProperty("organization", info.organization);
    obj.setProperty("firstName", info.firstName);
    obj.setProperty("lastName", info.lastName);
    obj.setProperty("email", info.email);
    return obj;
}

void participantInfoFromQScriptValue(const QScriptValue &scriptValue, Bodega::ParticipantInfo &info)
{
    info.assetCount = scriptValue.property("assetCount").toInt32();
    info.channelCount = scriptValue.property("channelCount").toInt32();
    info.pointsOwed = scriptValue.property("pointsOwed").toInt32();
    info.organization = scriptValue.property("organization").toString();
    info.firstName = scriptValue.property("firstName").toString();
    info.lastName = scriptValue.property("lastName").toString();
    info.email = scriptValue.property("email").toString();
}

BodegaStore::BodegaStore()
    : KDeclarativeMainWindow(),
      Bodega::CredentialsProvider(),
      m_historyModel(0),
      m_collectionsModel(0),
      m_collectionAssetsModel(0),
      m_participantRatingsModel(0),
      m_assetRatingsModel(0),
      m_bodegaUpdater(0)
{
    declarativeView()->setPackageName("com.makeplaylive.addonsapp");

    qmlRegisterType<Bodega::ParticipantInfoJob>();
    qmlRegisterType<Bodega::AssetJob>();
    qmlRegisterType<Bodega::AssetRatingsJob>();
    qmlRegisterType<Bodega::AssetOperations>();
    qmlRegisterType<Bodega::ChannelsJob>();
    qmlRegisterType<Bodega::HistoryModel>();
    qmlRegisterType<Bodega::Model>();
    qmlRegisterType<Bodega::NetworkJob>();
    qmlRegisterType<Bodega::RegisterJob>();
    qmlRegisterType<Bodega::Session>();
    qmlRegisterType<Bodega::SignOnJob>();
    qmlRegisterType<Bodega::InstallJob>();
    qmlRegisterType<Bodega::InstallJobsModel>();
    qmlRegisterType<Bodega::UninstallJob>();
    qmlRegisterType<Bodega::CollectionsModel>();
    qmlRegisterType<Bodega::CollectionAssetsModel>();
    qmlRegisterType<Bodega::ParticipantRatingsModel>();
    qmlRegisterType<Bodega::AssetRatingsModel>();
    qmlRegisterType<Bodega::UpdatedAssetsModel>();


    QDBusServiceWatcher *watcher = new QDBusServiceWatcher(s_bodegaUpdaterServiceName,
                                   QDBusConnection::sessionBus(),
                                   QDBusServiceWatcher::WatchForOwnerChange,
                                   this);
    QObject::connect(watcher, SIGNAL(serviceOwnerChanged(QString,QString,QString)),
                     this, SLOT(serviceChange(QString,QString,QString)));


    qRegisterMetaType<Bodega::InstallJobScheduler::InstallStatus>("Bodega::InstallJobScheduler::InstallStatus");
    qScriptRegisterMetaType<Bodega::InstallJobScheduler::InstallStatus>(declarativeView()->scriptEngine(), qScriptValueFromStatus, statusFromQScriptValue, QScriptValue());
    qmlRegisterUncreatableType<Bodega::InstallJobScheduler>("com.makeplaylive.addonsapp", 1, 0, "InstallJobScheduler", QLatin1String("Do not create objects of this type."));
    qmlRegisterUncreatableType<Bodega::ErrorCodes>("com.makeplaylive.addonsapp", 1, 0, "ErrorCode", QLatin1String("Do not create objects of this type."));
    qScriptRegisterMetaType<Bodega::Error>(declarativeView()->scriptEngine(), qScriptValueFromError, errorFromQScriptValue, QScriptValue());
    qScriptRegisterMetaType<Bodega::ChannelInfo>(declarativeView()->scriptEngine(), qScriptValueFromChannelInfo, channelInfoFromQScriptValue, QScriptValue());
    qScriptRegisterMetaType<Bodega::AssetInfo>(declarativeView()->scriptEngine(), qScriptValueFromAssetInfo, assetInfoFromQScriptValue, QScriptValue());
    qScriptRegisterMetaType<Bodega::Tags>(declarativeView()->scriptEngine(), qScriptValueFromTags, tagsFromQScriptValue, QScriptValue());
    qScriptRegisterMetaType<Bodega::ParticipantInfo>(declarativeView()->scriptEngine(), qScriptValueFromParticipantInfo, participantInfoFromQScriptValue, QScriptValue());

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    m_startPage = args->getOption("startpage");

    m_session = new Session(this);
    KConfigGroup config(KGlobal::config(), "AddOns");
    m_session->setBaseUrl(config.readEntry("URL", "https://addons.makeplaylive.com:3443"));
    m_session->setStoreId(config.readEntry("Store", "VIVALDI-1"));

    m_collectionsModel = new Bodega::CollectionsModel(this);
    m_collectionsModel->setSession(m_session);
    m_collectionAssetsModel = new Bodega::CollectionAssetsModel(this);
    m_collectionAssetsModel->setSession(m_session);
    m_channelsModel = new Bodega::Model(this);
    m_channelsModel->setSession(m_session);
    m_searchModel = new Bodega::Model(this);
    m_searchModel->setSession(m_session);

    declarativeView()->rootContext()->setContextProperty("bodegaClient", this);
}

BodegaStore::~BodegaStore()
{
}

Session* BodegaStore::session() const
{
    return m_session;
}

QString BodegaStore::startPage() const
{
    return m_startPage;
}

Model* BodegaStore::channelsModel() const
{
    return m_channelsModel;
}

Model* BodegaStore::searchModel() const
{
    return m_searchModel;
}

HistoryModel *BodegaStore::historyModel()
{
    if (!m_historyModel) {
        m_historyUsers = 0;
        m_historyModel = new HistoryModel(m_session);
        m_historyModel->setSession(m_session);
    }

    return m_historyModel;
}

CollectionsModel *BodegaStore::collectionsModel() const
{
    return m_collectionsModel;
}

CollectionAssetsModel *BodegaStore::collectionAssetsModel() const
{
    return m_collectionAssetsModel;
}

ParticipantRatingsModel *BodegaStore::participantRatingsModel()
{
    if (!m_participantRatingsModel) {
        m_participantRatingsModel = new ParticipantRatingsModel(this);
        m_participantRatingsModel->setSession(m_session);
    }
    return m_participantRatingsModel;
}

AssetRatingsModel *BodegaStore::assetRatingsModel()
{
    if (!m_assetRatingsModel) {
        m_assetRatingsModel = new AssetRatingsModel(this);
        m_assetRatingsModel->setSession(m_session);
    }
    return m_assetRatingsModel;
}

Bodega::UpdatedAssetsModel *BodegaStore::updatedAssetsModel() const
{
    return UpdatedAssetsModel::self();
}

Bodega::InstallJobScheduler *BodegaStore::installJobScheduler() const
{
    return InstallJobScheduler::self();
}

void BodegaStore::historyInUse(bool used)
{
    if (used) {
        ++m_historyUsers;
    } else {
        --m_historyUsers;
        if (m_historyUsers < 1) {
            m_historyUsers = 0;
            m_historyModel->deleteLater();
            m_historyModel = 0;
        }
    }
}

void BodegaStore::forgetCredentials() const
{
    m_session->setUserName(QString());
    m_session->setPassword(QString());
    saveCredentials();
}

void BodegaStore::authenticate(Bodega::Session *session)
{
    if (session->isAuthenticated()) {
        return;
    }

    QVariantHash credentials = retrieveCredentials(session);

    if (credentials.isEmpty()) {
        //FIXME: we need to be able to request the user/pass from the user
    } else {
        session->setUserName(credentials.value("username").toString());
        session->setPassword(credentials.value("password").toString());
        session->signOn();
    }
}

QString sessionWalletKey(Bodega::Session *session)
{
    return session->baseUrl().host() + "::" + session->baseUrl().port() + '_' + session->storeId();
}

void BodegaStore::saveCredentials() const
{
    KWallet::Wallet *wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),
                                           winId(), KWallet::Wallet::Synchronous);
    if (wallet->isOpen() &&
        (wallet->hasFolder("MakePlayLive") ||
         wallet->createFolder("MakePlayLive")) &&
         wallet->setFolder("MakePlayLive")) {

        QMap<QString, QString> map;
        map["username"] = m_session->userName();
        map["password"] = m_session->password();

        if (wallet->writeMap(sessionWalletKey(m_session), map) != 0) {
            kWarning() << "Unable to write credentials to wallet";
        }
    } else {
        kWarning() << "Unable to open wallet";
    }
}

QVariantHash BodegaStore::retrieveCredentials() const
{
    return retrieveCredentials(m_session);
}

QVariantHash BodegaStore::retrieveCredentials(Bodega::Session *session) const
{
    if (!session) {
        return QVariantHash();
    }

    //FIXME: synchronous API -> not good
    KWallet::Wallet *wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),
                                           winId(), KWallet::Wallet::Synchronous);
    if (wallet && wallet->isOpen() && wallet->setFolder("MakePlayLive")) {

        QMap<QString, QString> map;

        if (!wallet->readMap(sessionWalletKey(session), map)) {
            // compatibility mode for old storage ...
            wallet->readMap("credentials", map);
        }

        if (map.isEmpty()) {
            kWarning() << "Unable to read credentials from wallet";
        } else {
            QVariantHash hash;
            hash["username"] = map["username"];
            hash["password"] = map["password"];
            return hash;
        }
    } else {
        kWarning() << "Unable to open wallet";
    }

    return QVariantHash();
}

void BodegaStore::checkForUpdates()
{
    if (m_bodegaUpdater && m_bodegaUpdater->isValid()) {
        m_bodegaUpdater->call(QDBus::NoBlock, "checkForUpdates");
    }
}

void BodegaStore::serviceChange(const QString &name, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(name)
    if (newOwner.isEmpty()) {
        //unregistered
        kDebug() << "Connection to the BodegaUpdater lost";
        delete m_bodegaUpdater;
        m_bodegaUpdater = 0;
    } else if (oldOwner.isEmpty()) {
        //registered
        registerToDaemon();
    }
}

void BodegaStore::registerToDaemon()
{
    kDebug() << "Registering a client interface to the BodegaUpdater";
    if (!m_bodegaUpdater) {
        m_bodegaUpdater = new org::kde::BodegaUpdater(s_bodegaUpdaterServiceName, "/BodegaUpdater",
                                                      QDBusConnection::sessionBus());
    }

    if (m_bodegaUpdater->isValid()) {
        kDebug() << "Successfully registered to BodegaUpdater";
    } else {
        kDebug() << "BodegaUpdater not reachable";
        delete m_bodegaUpdater;
        m_bodegaUpdater = 0;
    }
}

#include "bodegastore.moc"
