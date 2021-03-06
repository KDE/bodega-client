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

#ifndef BODEGA_WALLPAPERINSTALLJOB_H
#define BODEGA_WALLPAPERINSTALLJOB_H

#include <bodega/installjob.h>
#include <bodega/globals.h>

namespace Bodega {

    class WallpaperHandler;

    class WallpaperInstallJob : public InstallJob
    {
        Q_OBJECT

    public:
        WallpaperInstallJob(QNetworkReply *reply, Session *session, WallpaperHandler *handler);
        ~WallpaperInstallJob();

    protected:
        void downloadFinished(const QString &localFile);

    private:
        WallpaperHandler *m_handler;
    };
}


#endif
