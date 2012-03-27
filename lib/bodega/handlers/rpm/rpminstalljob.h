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

#ifndef BODEGA_BOOKINSTALLJOB_H
#define BODEGA_BOOKINSTALLJOB_H

#include <bodega/installjob.h>
#include <bodega/globals.h>

#include <QPackageKit>

namespace Bodega {
    class RpmHandler;

    class RpmInstallJob : public InstallJob
    {
        Q_OBJECT

    public:
        RpmInstallJob(QNetworkReply *reply, Session *session, RpmHandler *handler);
        ~RpmInstallJob();

    protected:
        void downloadFinished(const QString &localFile);

    private Q_SLOTS:
        void errorOccurred(PackageKit::Enum::Error error, QString &message);
        void installFinished(PackageKit::Enum::Exit status, uint runtime);

    private:
        RpmHandler *m_handler;
    };
}


#endif
