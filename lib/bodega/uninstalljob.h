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

#ifndef BODEGA_UNINSTALLJOB_H
#define BODEGA_UNINSTALLJOB_H

#include <bodega/error.h>
#include <bodega/globals.h>
#include <bodega/networkjob.h>


namespace Bodega {

    class Session;

    class BODEGA_EXPORT UninstallJob : public NetworkJob
    {
        Q_OBJECT

    public:
        UninstallJob(Session *parent);
        ~UninstallJob();

    private:
        class Private;
        Private * const d;
    };
}

#endif
