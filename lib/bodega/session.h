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

class QAbstractItemModel;

namespace Bodega {

    class AssetJob;
    class AssetOperations;
    class BallotAddAssetJob;
    class BallotListAssetsJob;
    class BallotRemoveAssetJob;
    class ChannelsJob;
    class ChangeLanguageJob;
    class CreateBallotJob;
    class DeleteBallotJob;
    class InstallJob;
    class ListBallotsJob;
    class ParticipantInfoJob;
    class RegisterJob;
    class ResetPasswordJob;
    class SearchJob;
    class SignOnJob;
    class UninstallJob;

    class BODEGA_EXPORT Session : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl)
        Q_PROPERTY(bool authenticated READ isAuthenticated NOTIFY authenticated)
        Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
        Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
        Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceChanged)
        Q_PROPERTY(int points READ points NOTIFY pointsChanged)

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
        void setPassword(const QString &password);

        QString deviceId() const;
        void setDeviceId(const QString &device);

        QMap<ImageUrl, QUrl> imageUrls() const;
        QMap<ImageUrl, QUrl> urlsForImage(const QString &name) const;

    public Q_SLOTS:
        Bodega::NetworkJob *history(int offset, int pageSize);
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

        Bodega::NetworkJob *redeemPointsCode(const QString &code);

        Bodega::ListBallotsJob *listBallots(int offset=-1, int pageSize=-1);
        Bodega::CreateBallotJob *createCollection(
            const QString &name,
            Bodega::BallotInfo::BallotFlags flags = Bodega::BallotInfo::None);
        Bodega::DeleteBallotJob *deleteCollection(const QString &collectionId);
        Bodega::BallotAddAssetJob *collectionAddAsset(const QString &collectionId,
                                                  const QString &assetId);
        Bodega::BallotRemoveAssetJob *collectionRemoveAsset(const QString &collectionId,
                                                        const QString &assetId);
        Bodega::BallotListAssetsJob *collectionListAssets(const QString &collectionId,
                                                      int offset=-1,
                                                      int pageSize=-1);

        /*
         * These two are special because they don't require a session, as
         * such they're fire and forget jobs that don't require
         * authentication and don't set any state
         */
        Bodega::RegisterJob *registerAccount(
            const QString &email,
            const QString &password,
            const QString &firstName = QString(),
            const QString &middleNames = QString(),
            const QString &lastName = QString());
        Bodega::NetworkJob *resetPassword(const QString &email);
        Bodega::NetworkJob *changePassword(const QString &newPassword);
        Bodega::NetworkJob *changeAccountDetails(const QString &firstName, const QString &lastName, const QString &email);

    Q_SIGNALS:
        void disconnected();
        void authenticated(bool);
        void pointsChanged(int points);
        void userNameChanged();
        void passwordChanged();
        void deviceChanged();

    private:
        class Private;
        Private * const d;

        Q_PRIVATE_SLOT(d, void signOnFinished(Bodega::SignOnJob *job));
        Q_PRIVATE_SLOT(d, void jobFinished(Bodega::NetworkJob *job));
    };
}

#endif
