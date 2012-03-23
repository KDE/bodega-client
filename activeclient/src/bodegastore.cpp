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

#include <KDebug>
#include <kwallet.h>

//Bodega libs
#include <bodega/assetoperations.h>
#include <bodega/bodegamodel.h>
#include <bodega/channelsjob.h>
#include <bodega/networkjob.h>
#include <bodega/session.h>
#include <bodega/signonjob.h>

using namespace Bodega;

Q_DECLARE_METATYPE(Bodega::Error)
Q_DECLARE_METATYPE(Bodega::ChannelInfo)


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
    obj.setProperty("version", info.version);
    obj.setProperty("path", info.path.toString());
    //obj.setProperty("images", info.images);
    obj.setProperty("description", info.description);
    obj.setProperty("points", info.points);

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
        } else if (it.name() == "version") {
            info.version = it.value().toString();
        } else if (it.name() == "path") {
            info.path = it.value().toString();
        } else if (it.name() == "description") {
            info.description = it.value().toString();
        } else if (it.name() == "points") {
            info.points = it.value().toInteger();
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



BodegaStore::BodegaStore()
    : KDeclarativeMainWindow()
{

    declarativeView()->setPackageName("com.coherenttheory.bodegastore");

    qmlRegisterType<Bodega::AssetJob>();
    qmlRegisterType<Bodega::AssetOperations>();
    qmlRegisterType<Bodega::ChannelsJob>();
    qmlRegisterType<Bodega::Model>();
    qmlRegisterType<Bodega::NetworkJob>();
    qmlRegisterType<Bodega::Session>();
    qmlRegisterType<Bodega::SignOnJob>();

    qScriptRegisterMetaType<Bodega::Error>(declarativeView()->scriptEngine(), qScriptValueFromError, errorFromQScriptValue, QScriptValue());
    qScriptRegisterMetaType<Bodega::ChannelInfo>(declarativeView()->scriptEngine(), qScriptValueFromChannelInfo, channelInfoFromQScriptValue, QScriptValue());
    qScriptRegisterMetaType<Bodega::AssetInfo>(declarativeView()->scriptEngine(), qScriptValueFromAssetInfo, assetInfoFromQScriptValue, QScriptValue());
    qScriptRegisterMetaType<Bodega::Tags>(declarativeView()->scriptEngine(), qScriptValueFromTags, tagsFromQScriptValue, QScriptValue());

    m_session = new Session(this);
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

void BodegaStore::saveCredentials(const QString &username, const QString &password) const
{
    KWallet::Wallet *wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),
                                           winId(), KWallet::Wallet::Synchronous);
    if (wallet->isOpen() &&
        (wallet->hasFolder("Bodega") ||
         wallet->createFolder("Bodega")) &&
         wallet->setFolder("Bodega")) {

        QMap<QString, QString> map;
        map["username"] = username;
        map["password"] = password;

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
    if (wallet->isOpen() && wallet->setFolder("Bodega")) {

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
