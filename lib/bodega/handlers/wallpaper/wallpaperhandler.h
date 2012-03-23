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


#ifndef WALLPAPEROPERATIONS_H
#define WALLPAPEROPERATIONS_H

#include <bodega/assethandler.h>

#include <qplugin.h>

namespace Bodega {

    class WallpaperHandler : public AssetHandler
    {
        Q_OBJECT
        Q_INTERFACES(Bodega::AssetHandler)

    public:
        WallpaperHandler(QObject *parent = 0);
        ~WallpaperHandler();

        QString launchText() const;
        bool isInstalled() const;

    public Q_SLOTS:
        Bodega::InstallJob *install(QNetworkReply *reply, Session *session);
        Bodega::UninstallJob *uninstall();
        void launch();

    private:
        class Private;
        Private * const d;
    };

}

Q_EXPORT_PLUGIN2(wallpaperhandler, Bodega::WallpaperHandler);

#endif
