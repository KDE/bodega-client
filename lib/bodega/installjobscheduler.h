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

#ifndef BODEGA_INSTALLJOBSCHEDULER_H
#define BODEGA_INSTALLJOBSCHEDULER_H

#include <QObject>

#include <bodega/globals.h>

namespace Bodega
{
    class Session;
    class InstallJob;

    class BODEGA_EXPORT InstallJobScheduler : public QObject
    {
        Q_OBJECT

    public:
        enum InstallStatus
        {
            Idle = 0,
            Waiting,
            Installing,
            Finished
        };
        //singleton instead?
        InstallJobScheduler(QObject *parent);
        ~InstallJobScheduler();

        //not an AssetOperation since it has to be taken asynchronously too
        //passing session so it can work with multiple warehouses too
        void scheduleInstall(const QString &assetId, Bodega::Session *sesison);

        InstallStatus installStatus(const QString &assetId);

        Bodega::InstallJob *installJobForAsset(const QString &assetId);

    Q_SIGNALS:
        void installStatusChanged(const QString &assetId, InstallStatus status);

    private:
        class Private;
        Private * const d;

        Q_PRIVATE_SLOT(d, void jobFinished(Bodega::NetworkJob *))
        Q_PRIVATE_SLOT(d, void operationReady())
    };
}

#endif
