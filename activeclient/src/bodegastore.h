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


#ifndef BODEGASTORE_H
#define BODEGASTORE_H

#include "bodega/credentialsprovider.h"

#include "kdeclarativemainwindow.h"
#include "bodegaupdater_interface.h"


namespace Bodega {
    class HistoryModel;
    class Model;
    class Session;
    class CollectionsModel;
    class CollectionAssetsModel;
    class ParticipantRatingsJobModel;
    class AssetRatingsJobModel;
    class UpdatedAssetsModel;
    class InstallJobScheduler;
}

class BodegaStore : public KDeclarativeMainWindow, public Bodega::CredentialsProvider
{
    Q_OBJECT
    Q_PROPERTY(Bodega::Session *session READ session CONSTANT)
    Q_PROPERTY(QString startPage READ startPage CONSTANT)
    Q_PROPERTY(Bodega::Model *channelsModel READ channelsModel CONSTANT)
    Q_PROPERTY(Bodega::Model *searchModel READ searchModel CONSTANT)
    Q_PROPERTY(Bodega::HistoryModel *historyModel READ historyModel CONSTANT)
    Q_PROPERTY(Bodega::CollectionsModel *collectionsModel READ collectionsModel CONSTANT)
    Q_PROPERTY(Bodega::CollectionAssetsModel *collectionAssetsModel READ collectionAssetsModel CONSTANT)
    Q_PROPERTY(Bodega::ParticipantRatingsJobModel *participantRatingsJobModel READ participantRatingsJobModel CONSTANT)
    Q_PROPERTY(Bodega::AssetRatingsJobModel *assetRatingsJobModel READ assetRatingsJobModel CONSTANT)
    Q_PROPERTY(Bodega::UpdatedAssetsModel *updatedAssetsModel READ updatedAssetsModel CONSTANT)
    Q_PROPERTY(Bodega::InstallJobScheduler *installJobScheduler READ installJobScheduler CONSTANT)

public:
    BodegaStore();
    virtual ~BodegaStore();


    Bodega::Session *session() const;
    Bodega::Model* channelsModel() const;
    Bodega::Model* searchModel() const;
    Bodega::HistoryModel *historyModel();
    Bodega::CollectionsModel *collectionsModel() const;
    Bodega::CollectionAssetsModel *collectionAssetsModel() const;
    Bodega::ParticipantRatingsJobModel *participantRatingsJobModel();
    Bodega::AssetRatingsJobModel *assetRatingsJobModel();
    Bodega::UpdatedAssetsModel *updatedAssetsModel() const;
    Bodega::InstallJobScheduler *installJobScheduler() const;
    QString startPage() const;

    // the authenticate method is from the CredentialsProvider interface
    void authenticate(Bodega::Session *session);
    Q_INVOKABLE void saveCredentials() const;
    Q_INVOKABLE void forgetCredentials() const;
    Q_INVOKABLE QVariantHash retrieveCredentials() const;
    Q_INVOKABLE QVariantHash retrieveCredentials(Bodega::Session *session) const;
    Q_INVOKABLE void historyInUse(bool used);
    Q_INVOKABLE void checkForUpdates();

private Q_SLOTS:
    void registerToDaemon();
    void serviceChange(const QString &name, const QString &oldOwner, const QString &newOwner);

private:
    Bodega::Session *m_session;
    Bodega::Model *m_channelsModel;
    Bodega::Model *m_searchModel;
    Bodega::HistoryModel *m_historyModel;
    Bodega::CollectionsModel *m_collectionsModel;
    Bodega::CollectionAssetsModel *m_collectionAssetsModel;
    Bodega::ParticipantRatingsJobModel *m_participantRatingsJobModel;
    Bodega::AssetRatingsJobModel *m_assetRatingsJobModel;
    org::kde::BodegaUpdater *m_bodegaUpdater;
    int m_historyUsers;
    QString m_startPage;
};

#endif // BODEGASTORE_H
