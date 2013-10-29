/*
 *   Copyright 2013 Coherent Theory LLC
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

#ifndef BODEGA_GENERICINSTALLJOB_H
#define BODEGA_GENERICINSTALLJOB_H

#include <bodega/error.h>
#include <bodega/installjob.h>

namespace Bodega {

    class GenericHandler;
    class GenericInstallJob : public InstallJob
    {
        Q_OBJECT

    public:
        GenericInstallJob(QNetworkReply *reply, Session *session, GenericHandler *handler);
        ~GenericInstallJob();

    protected:
        void downloadFinished(const QString &localFile);

    private Q_SLOTS:
        void threadFinished(const Bodega::Error &error);

    private:
        GenericHandler *m_handler;
    };
}


#endif
