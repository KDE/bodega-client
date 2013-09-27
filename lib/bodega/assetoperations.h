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


#ifndef ASSETOPERATIONINTERFACE_H
#define ASSETOPERATIONINTERFACE_H

#include <QObject>

#include <bodega/globals.h>
#include <bodega/session.h>

class QAbstractItemModel;

namespace Bodega {
    class InstallJob;
    class UninstallJob;

    class BODEGA_EXPORT AssetOperations : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString launchText READ launchText NOTIFY ready)
        Q_PROPERTY(bool installed READ isInstalled NOTIFY installedChanged)
        Q_PROPERTY(bool ready READ isReady NOTIFY ready)
        Q_PROPERTY(Bodega::ChangeLog changeLog READ changeLog NOTIFY infoReady)
        Q_PROPERTY(Bodega::AssetInfo assetInfo READ assetInfo NOTIFY infoReady)
        Q_PROPERTY(Bodega::Tags assetTags READ assetTags NOTIFY infoReady)
        Q_PROPERTY(QString mimetype READ mimetype)
        Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
        Q_PROPERTY(QAbstractItemModel *ratingsModel READ ratingsModel CONSTANT)

    public:
        AssetOperations(const QString &assetId, Session *parent);
        ~AssetOperations();

        const Bodega::ChangeLog& changeLog() const;
        const Bodega::AssetInfo& assetInfo() const;
        const Bodega::Tags& assetTags() const;

        bool isReady() const;

        QString launchText() const;
        bool isInstalled() const;

        QString mimetype() const;

        qreal progress() const;

        QAbstractItemModel *ratingsModel();

    public Q_SLOTS:
        Bodega::InstallJob *install(QNetworkReply *reply, Session *session);
        Bodega::UninstallJob *uninstall(Session *session);
        void launch();

    Q_SIGNALS:
        void failed();
        void ready();
        void infoReady();
        void installedChanged();
        void progressChanged(qreal progress);

    private:
        class Private;
        Private * const d;

        Q_PRIVATE_SLOT(d, void assetDownloadComplete(Bodega::NetworkJob*));
        Q_PRIVATE_SLOT(d, void checkInstalled());
        Q_PRIVATE_SLOT(d, void progressHasChanged(qreal progress));
    };

}

#endif
