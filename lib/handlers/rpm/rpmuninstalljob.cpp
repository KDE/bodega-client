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

#include "rpmuninstalljob.h"
#include "rpmhandler.h"
#include "session.h"

#include <QDebug>
#include <QFile>

using namespace Bodega;

RpmUninstallJob::RpmUninstallJob(Session *parent, RpmHandler *handler)
    : UninstallJob(parent),
      m_handler(handler)
{
    PackageKit::Transaction *transaction = new PackageKit::Transaction(this);
    
    if (!handler->package().id().isEmpty()) {
        transaction->simulateRemovePackage(handler->package(), true);
        connect(transaction, SIGNAL(errorCode(PackageKit::Transaction::Error, QString)),
                this, SLOT(errorOccurred(PackageKit::Transaction::Error, QString)));
        connect(transaction, SIGNAL(finished(PackageKit::Transaction::Exit, uint)),
                this, SLOT(simulateUninstallFinished(PackageKit::Transaction::Exit, uint)));
    } else {
        setError(Error(Error::Session,
                       QLatin1String("rpm/01"),
                       tr("Uninstall failed"),
                       tr("Package not installed.")));
        setFinished();
    }
}

RpmUninstallJob::~RpmUninstallJob()
{
}

void RpmUninstallJob::simulateUninstallFinished(PackageKit::Transaction::Exit status, uint runtime)
{
    PackageKit::Transaction *transaction = new PackageKit::Transaction(this);

    transaction->removePackage(m_handler->package(), true, true);
    connect(transaction, SIGNAL(errorCode(PackageKit::Transaction::Error, QString)),
            this, SLOT(errorOccurred(PackageKit::Transaction::Error, QString)));
    connect(transaction, SIGNAL(finished(PackageKit::Transaction::Exit, uint)),
            this, SLOT(uninstallFinished(PackageKit::Transaction::Exit, uint)));

}

void RpmUninstallJob::errorOccurred(PackageKit::Transaction::Error error, const QString &message)
{
    setError(Error(Error::Session,
                   QString(QLatin1String("rpm/%1")).arg(error),
                   tr("Uninstall failed"),
                   message));
}

void RpmUninstallJob::uninstallFinished(PackageKit::Transaction::Exit status, uint runtime)
{
    setFinished();
}

#include "rpmuninstalljob.moc"
