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

#ifndef BODEGA_SESSION_H
#define BODEGA_SESSION_H

#include <bodega/globals.h>

//for AssetFlags
#include <bodega/assetjob.h>

#include <QtCore/QObject>
#include <QtCore/QUrl>

namespace Bodega {

    class AssetJob;
    class AssetOperations;
    class ChannelsJob;
    class ChangeLanguageJob;
    class InstallJob;
    class ParticipantInfoJob;
    class RegisterJob;
    class SearchJob;
    class SignOnJob;
    class UninstallJob;

    class BODEGA_EXPORT Session : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl)
        Q_PROPERTY(bool authenticated READ isAuthenticated NOTIFY authenticated)
        Q_PROPERTY(QString userName READ userName WRITE setUserName)
        Q_PROPERTY(QString password READ password WRITE setPassword)
        Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId)
        //TODO: needs a pointsChanged signal probably
        Q_PROPERTY(int points READ points)

    public:
        Session(QObject *parent=0);
        ~Session();

        QUrl baseUrl() const;
        void setBaseUrl(const QUrl &url);

        bool isAuthenticated() const;

        int points() const;

        QString userName() const;
        void setUserName(const QString &name);

        QString password() const;
        void setPassword(const QString &pass);

        QString deviceId() const;
        void setDeviceId(const QString &device);

        QMap<ImageUrl, QUrl> imageUrls() const;
        QMap<ImageUrl, QUrl> urlsForImage(const QString &name) const;

    public Q_SLOTS:
        Bodega::SignOnJob *signOn();
        void signOut();
        Bodega::ChannelsJob *channels(const QString &topChannel=QString(),
                                      int offset=-1, int pageSize=-1);

        // continues listing channels from where the passed job
        // stopped
        Bodega::ChannelsJob *nextChannels(const ChannelsJob *job);
        Bodega::ChannelsJob *prevChannels(const ChannelsJob *job);

        Bodega::AssetJob *asset(const QString &assetId,
                                AssetJob::AssetFlags flags=AssetJob::None);
        Bodega::ChangeLanguageJob *changeLanguage(const QString &lang);
        Bodega::ChannelsJob *search(const QString &text,
                                    const QString &channelId,
                                    int offset=-1, int pageSize=-1);
        Bodega::ParticipantInfoJob *participantInfo();

        Bodega::AssetOperations *assetOperations(const QString &assetId);
        Bodega::InstallJob *install(Bodega::AssetOperations *operations);
        Bodega::UninstallJob *uninstall(Bodega::AssetOperations *operations);

        Bodega::RegisterJob *registerAccount(
            const QString &email,
            const QString &password,
            const QString &firstName = QString(),
            const QString &middleNames = QString(),
            const QString &lastName = QString());

    Q_SIGNALS:
        void disconnected();
        void authenticated(bool);

    private:
        class Private;
        Private * const d;

        Q_PRIVATE_SLOT(d, void signOnFinished(Bodega::SignOnJob *job));
        Q_PRIVATE_SLOT(d, void jobFinished(Bodega::NetworkJob *job));
    };
}

#endif
