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

#include <bodega/error.h>
#include <bodega/globals.h>
#include <bodega/installjob.h>

namespace Bodega {
    class BookHandler;

    class BODEGA_EXPORT BookInstallJob : public InstallJob
    {
        Q_OBJECT

    public:
        BookInstallJob(QNetworkReply *reply, Session *session, BookHandler *handler);
        ~BookInstallJob();

    protected:
        void downloadFinished(const QString &localFile);

    private Q_SLOTS:
        void threadCompleted(const Bodega::Error &error);

    private:
        BookHandler *m_handler;
    };
}


#endif
