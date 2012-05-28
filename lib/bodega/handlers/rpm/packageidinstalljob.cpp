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


#include "packageidinstalljob.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "assetoperations.h"
#include "rpmhandler.h"

namespace Bodega
{

PackageIdInstallJob::PackageIdInstallJob(QNetworkReply *reply, Session *session,
                               RpmHandler *handler)
    : InstallJob(reply, session)
{
    m_handler = handler;
}

PackageIdInstallJob::~PackageIdInstallJob()
{
}

void PackageIdInstallJob::downloadFinished(const QString &localFile)
{
    QFile idFile(localFile);

    QString pkgId;
    if (!idFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError(Error(Error::Session,
                   QString(QLatin1String("packageid/1")),
                   tr("Install error"),
                   tr("failed to open the descriptor file")));
        return;
    }

    while (!idFile.atEnd()) {
        pkgId = QLatin1String(idFile.readLine());
        if(pkgId.split(QLatin1Char(';'), QString::KeepEmptyParts).count() == 4) {
            m_packageId = pkgId;
            break;
        }
    }
    idFile.close();
    if (m_packageId.isEmpty()) {
        setError(Error(Error::Session,
                   QString(QLatin1String("packageid/2")),
                   tr("Install error"),
                   tr("failed to parse the descriptor file")));
        return;
    }

    qDebug() << "Simulate install" << m_packageId;

    PackageKit::Transaction *transaction = new PackageKit::Transaction(this);
    transaction->simulateInstallPackage(PackageKit::Package(m_packageId));

    connect(transaction, SIGNAL(errorCode(PackageKit::Transaction::Error, QString)),
            this, SLOT(errorOccurred(PackageKit::Transaction::Error, QString)));
    connect(transaction, SIGNAL(finished(PackageKit::Transaction::Exit, uint)),
            this, SLOT(simulateInstallFinished(PackageKit::Transaction::Exit, uint)));
    connect(transaction, SIGNAL(changed()),
            this, SLOT(transactionChanged()));
}


void PackageIdInstallJob::simulateInstallFinished(PackageKit::Transaction::Exit status, uint runtime)
{
    if (status == PackageKit::Transaction::ExitSuccess) {
        qDebug() << "Trying to install" << m_packageId;

        PackageKit::Transaction *transaction = new PackageKit::Transaction(this);
        transaction->installPackage(PackageKit::Package(m_packageId), false);

        connect(transaction, SIGNAL(errorCode(PackageKit::Transaction::Error, QString)),
                this, SLOT(errorOccurred(PackageKit::Transaction::Error, QString)));
        connect(transaction, SIGNAL(finished(PackageKit::Transaction::Exit, uint)),
                this, SLOT(installFinished(PackageKit::Transaction::Exit, uint)));
        connect(transaction, SIGNAL(changed()),
            this, SLOT(transactionChanged()));
    }
}

void PackageIdInstallJob::transactionChanged()
{
    PackageKit::Transaction *transaction = qobject_cast<PackageKit::Transaction *>(sender());
    if (!transaction) {
        return;
    }

    setProgress((qreal)transaction->percentage()/(qreal)100);
}

void PackageIdInstallJob::errorOccurred(PackageKit::Transaction::Error error, const QString &message)
{
    qDebug() << "Failed with error:" << error << message;
    setError(Error(Error::Session,
                   QString(QLatin1String("rpm/%1")).arg(error),
                   tr("Install failed"),
                   message));
    setFinished();
}

void PackageIdInstallJob::installFinished(PackageKit::Transaction::Exit status, uint runtime)
{
    qDebug() << "Job finished, exit code:" << status << "Running time:" << runtime;
    setFinished();
}

}

#include "packageidinstalljob.moc"

