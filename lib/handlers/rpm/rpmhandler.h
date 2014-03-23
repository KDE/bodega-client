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


#ifndef RPMOPERATIONS_H
#define RPMOPERATIONS_H

#include <bodega/assethandler.h>

#include <qplugin.h>

#include <Transaction>

#include "rpminstalljob.h"
#include "rpmuninstalljob.h"

namespace Bodega {

    class RpmHandler : public AssetHandler
    {
        Q_OBJECT
        Q_INTERFACES(Bodega::AssetHandler)

    public:
        RpmHandler(QObject *parent = 0);
        ~RpmHandler();

        void init();

        QString launchText() const;
        bool isInstalled() const;

        QString remoteName() const;
        bool remoteNameIsDescriptor() const;
        QString packageName() const;
        QString packageId() const;

    public Q_SLOTS:
        Bodega::InstallJob *install(QNetworkReply *reply, Session *session);
        Bodega::UninstallJob *uninstall(Session *session);
        void launch();

    private Q_SLOTS:
        void gotPackage(PackageKit::Transaction::Info info, const QString &packageId, const QString &summary);
        void resolveFinished(PackageKit::Transaction::Exit status, uint runtime);
        void gotFiles(const QString &package, const QStringList &filenames);
        void installJobFinished();

    private:
        QWeakPointer<InstallJob> m_installJob;
        QWeakPointer<RpmUninstallJob> m_uninstallJob;
        QString m_packageId;
        PackageKit::Transaction::Info m_info;
        PackageKit::Transaction *m_transaction;
        QString m_desktopFile;
    };
}

#endif
