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

#include <QtCore/QObject>

namespace Bodega {

    class Session;

    class BODEGA_EXPORT UninstallJob : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(bool finished READ isFinished NOTIFY finishedChanged())
        Q_PROPERTY(bool failed READ failed NOTIFY failedChanged())

    public:
        UninstallJob(Session *parent);
        ~UninstallJob();

        Session *session() const;

        bool isFinished() const;

        bool failed() const;
        Error error() const;

    protected:
        void setError(const Bodega::Error &e);
        void setFinished();

    Q_SIGNALS:
        void error(Bodega::UninstallJob *job, const Bodega::Error &error);
        void jobFinished(Bodega::UninstallJob *job);
        void failedChanged();
        void finishedChanged();


    private:
        class Private;
        Private * const d;
    };
}

#endif
