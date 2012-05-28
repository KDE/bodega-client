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

#ifndef BODEGA_PACKAGEIDINSTALLJOB_H
#define BODEGA_PACKAGEIDINSTALLJOB_H

#include <bodega/installjob.h>
#include <bodega/globals.h>

#include <Transaction>

namespace Bodega {
    class RpmHandler;

    class PackageIdInstallJob : public InstallJob
    {
        Q_OBJECT

    public:
        PackageIdInstallJob(QNetworkReply *reply, Session *session, RpmHandler *handler);
        ~PackageIdInstallJob();

    protected:
        void downloadFinished(const QString &localFile);

    private Q_SLOTS:
        void errorOccurred(PackageKit::Transaction::Error error, const QString &message);
        void simulateInstallFinished(PackageKit::Transaction::Exit status, uint runtime);
        void installFinished(PackageKit::Transaction::Exit status, uint runtime);

    private:
        RpmHandler *m_handler;
        QString m_packageId;
    };
}


#endif
