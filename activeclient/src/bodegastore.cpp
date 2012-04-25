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

#include "appbackgroundprovider_p.h"
#include "bodegastore.h"
#include "kdeclarativeview.h"

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QScriptEngine>
#include <QScriptValueIterator>

#include <KConfig>
#include <KConfigGroup>
#include <KDebug>
#include <KGlobal>
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
#include <bodega/uninstalljob.h>

using namespace Bodega;

QScriptValue qScriptValueFromError(QScriptEngine *engine, const Bodega::Error &error)
{
    QScriptValue obj = engine->newObject();

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
        if (it.name() == "type") {
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

    QScriptValue imageObj = engine->newObject();
    imageObj.setProperty("tiny", info.images[ImageTiny].toString());
    imageObj.setProperty("small", info.images[ImageSmall].toString());
    imageObj.setProperty("medium", info.images[ImageMedium].toString());
    imageObj.setProperty("large", info.images[ImageLarge].toString());
    imageObj.setProperty("huge", info.images[ImageHuge].toString());
    imageObj.setProperty("previews", info.images[ImagePreviews].toString());
    obj.setProperty("images", imageObj);

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
                } else if (imageIt.name() == "large") {
                    images[ImageLarge] = imageIt.value().toString();
                } else if (imageIt.name() == "huge") {
                    images[ImageHuge] = imageIt.value().toString();
                } else if (imageIt.name() == "previews") {
                    images[ImagePreviews] = imageIt.value().toString();
                }
            }
            info.images = images;
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
      m_historyModel(0)
{
    // For kde-runtime 4.8 compabitility, the appbackgrounds image provider is only
    // in PlasmaExtras 4.9 (master currently)
    // FIXME: Remove this call and the class from commmon/ once we can depend on 4.9
    if (!declarativeView()->engine()->imageProvider("appbackgrounds")) {
        declarativeView()->engine()->addImageProvider(QLatin1String("appbackgrounds"), new AppBackgroundProvider);
    }
    declarativeView()->setPackageName("com.coherenttheory.addonsapp");

    qmlRegisterType<Bodega::ParticipantInfoJob>();
    qmlRegisterType<Bodega::AssetJob>();
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

    qScriptRegisterMetaType<Bodega::Error>(declarativeView()->scriptEngine(), qScriptValueFromError, errorFromQScriptValue, QScriptValue());
    qScriptRegisterMetaType<Bodega::ChannelInfo>(declarativeView()->scriptEngine(), qScriptValueFromChannelInfo, channelInfoFromQScriptValue, QScriptValue());
    qScriptRegisterMetaType<Bodega::AssetInfo>(declarativeView()->scriptEngine(), qScriptValueFromAssetInfo, assetInfoFromQScriptValue, QScriptValue());
    qScriptRegisterMetaType<Bodega::Tags>(declarativeView()->scriptEngine(), qScriptValueFromTags, tagsFromQScriptValue, QScriptValue());
    qScriptRegisterMetaType<Bodega::ParticipantInfo>(declarativeView()->scriptEngine(), qScriptValueFromParticipantInfo, participantInfoFromQScriptValue, QScriptValue());


    m_session = new Session(this);
    KConfigGroup config(KGlobal::config(), "AddOns");
    m_session->setBaseUrl(config.readEntry("URL", "http://addons.makeplaylive.com:3000"));
    m_session->setDeviceId(config.readEntry("Device", "VIVALDI-1"));


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

        if (wallet->writeMap("credentials", map) != 0) {
            kWarning() << "Unable to write credentials to wallet";
        }
    } else {
        kWarning() << "Unable to open wallet";
    }
}

QVariantHash BodegaStore::retrieveCredentials() const
{
    KWallet::Wallet *wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),
                                           winId(), KWallet::Wallet::Synchronous);
    if (wallet && wallet->isOpen() && wallet->setFolder("MakePlayLive")) {

        QMap<QString, QString> map;

        if (wallet->readMap("credentials", map) == 0) {
            QVariantHash hash;
            hash["username"] = map["username"];
            hash["password"] = map["password"];
            return hash;
        } else {
            kWarning() << "Unable to write credentials to wallet";
        }
    } else {
        kWarning() << "Unable to open wallet";
    }

    return QVariantHash();
}

#include "bodegastore.moc"
