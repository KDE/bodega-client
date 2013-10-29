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

#ifndef GENERICUNINSTALLJOB_H
#define GENERICUNINSTALLJOB_H

#include <bodega/error.h>
#include <bodega/uninstalljob.h>

namespace Bodega {

    class GenericHandler;
    class Session;

    class GenericUninstallJob : public UninstallJob
    {
        Q_OBJECT
    public:
        GenericUninstallJob(Session *parent, GenericHandler *handler);
        ~GenericUninstallJob();

    private Q_SLOTS:
        void start();
        void threadFinished(const Bodega::Error &error);

    private:
        GenericHandler *m_handler;
    };
}

#endif
