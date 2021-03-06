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


#ifndef BOOKOPERATIONS_H
#define BOOKOPERATIONS_H

#include <bodega/assethandler.h>

#include <qplugin.h>

#include "bookinstalljob.h"
#include "bookuninstalljob.h"

namespace Bodega {

    class BookHandler : public AssetHandler
    {
        Q_OBJECT
        Q_INTERFACES(Bodega::AssetHandler)

    public:
        BookHandler(QObject *parent = 0);
        ~BookHandler();

        QString launchText() const;
        bool isInstalled() const;

        QString filePath() const;

    public Q_SLOTS:
        Bodega::InstallJob *install(QNetworkReply *reply, Session *session);
        Bodega::UninstallJob *uninstall(Session *session);
        void launch();

    private:
        QWeakPointer<BookInstallJob> m_installJob;
        QWeakPointer<BookUninstallJob> m_uninstallJob;
    };
}

#endif
