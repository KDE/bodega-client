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
    class AssetBriefsJob;
    class AssetOperations;
    class collectionAddAssetJob;
    class collectionListAssetsJob;
    class collectionRemoveAssetJob;
    class ChannelsJob;
    class ChangeLanguageJob;
    class CreatecollectionJob;
    class DeletecollectionJob;
    class InstallJob;
    class InstallJobsModel;
    class ListCollectionsJob;
    class ParticipantInfoJob;
    class RegisterJob;
    class ResetPasswordJob;
    class SignOnJob;
    class UninstallJob;
    class RatingAttributesJob;
    class AssetRatingsJob;
    class ParticipantRatingsJob;
    class UpdatesCheckJob;

    class BODEGA_EXPORT Session : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl)
        Q_PROPERTY(bool authenticated READ isAuthenticated NOTIFY authenticated)
        Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
        Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
        Q_PROPERTY(QString storeId READ storeId WRITE setStoreId NOTIFY storeIdChanged)

        /**
         * Model containing all the install jobs for each asset done during this session
         */
        Q_PROPERTY(Bodega::InstallJobsModel *installJobsModel READ installJobsModel CONSTANT)
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

        QString storeId() const;
        void setStoreId(const QString &id);

        QMap<ImageUrl, QUrl> imageUrls() const;
        QMap<ImageUrl, QUrl> urlsForImage(const QString &name) const;

        Bodega::InstallJobsModel *installJobsModel() const;

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
        /**
         * Creates a job that retrives brief information on 1 or more assets
         * 
         * @args assetIds a list of asset ids to fetch information for
         */
        Bodega::AssetBriefsJob *assetBriefs(const QStringList &assetIds);
        Bodega::ChangeLanguageJob *changeLanguage(const QString &lang);
        Bodega::ChannelsJob *search(const QString &text,
                                    const QString &channelId,
                                    int offset=-1, int pageSize=-1);
        Bodega::ParticipantInfoJob *participantInfo();

        Bodega::AssetOperations *assetOperations(const QString &assetId);
        Bodega::InstallJob *install(Bodega::AssetOperations *operations);
        Bodega::UninstallJob *uninstall(Bodega::AssetOperations *operations);

        Bodega::NetworkJob *redeemPointsCode(const QString &code);
        Bodega::NetworkJob *buyPoints(int points);
        Bodega::NetworkJob *pointsPrice(int points);
        Bodega::NetworkJob *purchaseAsset(const QString &assetId);

        Bodega::NetworkJob *paymentMethod();
        Bodega::NetworkJob *deletePaymentMethod();
        Bodega::NetworkJob *setPaymentMethod(const QString &number, const QString &expiryMonth,
                                             const QString &expiryYear, const QString &cvc, const QString &name,
                                             const QString &address1 = QString(), const QString &address2 = QString(),
                                             const QString &addressZip = QString(), const QString &addressState = QString(),
                                             const QString &addressCountry = QString());

        Bodega::ListCollectionsJob *listcollections(int offset=-1, int pageSize=-1);
        Bodega::CreatecollectionJob *createCollection(
            const QString &name,
            Bodega::CollectionInfo::CollectionFlags flags = Bodega::CollectionInfo::None);
        Bodega::DeletecollectionJob *deleteCollection(const QString &collectionId);
        Bodega::collectionAddAssetJob *collectionAddAsset(const QString &collectionId,
                                                  const QString &assetId);
        Bodega::collectionRemoveAssetJob *collectionRemoveAsset(const QString &collectionId,
                                                        const QString &assetId);
        Bodega::collectionListAssetsJob *collectionListAssets(const QString &collectionId,
                                                      int offset=-1,
                                                      int pageSize=-1);

        Bodega::RatingAttributesJob *listRatingAttributes(const QString &assetId);
        Bodega::NetworkJob *deleteAssetRatings(const QString &assetId);
        Bodega::AssetRatingsJob *assetRatings(const QString &assetId);
        Bodega::ParticipantRatingsJob *participantRatings(int offset = -1, int pageSize = -1);
        Bodega::ParticipantRatingsJob *participantRatings(const QString &assetId);
        Bodega::NetworkJob *assetCreateRatings(const QString &assetId, const QVariantMap &ratings);
        Bodega::NetworkJob *assetDeleteRatings(const QString &assetId);
        /*
         * These methods are special because they don't require a session, as
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

        /**
         * Creates an update check. Does not require authentication.
         *
         * @arg assets a list of pairs of asset names and version timestamps to use in the check
         */
        Bodega::UpdatesCheckJob *updatesCheck(const QList<QPair<QString, QString> > &assets);

    Q_SIGNALS:
        void disconnected();
        void authenticated(bool);
        void pointsChanged(int points);
        void userNameChanged();
        void passwordChanged();
        void storeIdChanged();

    private:
        class Private;
        Private * const d;

        Q_PRIVATE_SLOT(d, void signOnFinished(Bodega::SignOnJob *job));
        Q_PRIVATE_SLOT(d, void jobFinished(Bodega::NetworkJob *job));
    };
}

Q_DECLARE_METATYPE(Bodega::Session *);
#endif
