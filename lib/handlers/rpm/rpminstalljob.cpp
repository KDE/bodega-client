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


#include "rpminstalljob.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "assetoperations.h"
#include "rpmhandler.h"

namespace Bodega
{

RpmInstallJob::RpmInstallJob(QNetworkReply *reply, Session *session, RpmHandler *handler)
    : InstallJob(reply, session)
{
    m_handler = handler;
}

RpmInstallJob::~RpmInstallJob()
{
}

void RpmInstallJob::downloadFinished(const QString &localFile)
{
    QFileInfo remoteInfo(localFile);
    m_packagePath = QDir::tempPath() + QDir::separator() + m_handler->remoteName();
    QFile f(localFile);

    QFile oldFile(m_packagePath);
    if (oldFile.exists()) {
        oldFile.remove();
    }
    //RPM uses the file name to know what package actually is
    f.rename(m_packagePath);

    qDebug() << "Getting rid of PK_TMP_DIR if exists";
    PackageKit::Transaction *transaction = new PackageKit::Transaction(this);
    transaction->repoSetData(QLatin1String("PK_TMP_DIR"), QLatin1String("remove"), QLatin1String("true"));
    //if the repo was not existent, is ok go forward with install procedure
    connect(transaction, SIGNAL(errorCode(PackageKit::Transaction::Error, QString)),
            this, SLOT(removeRepoFinished()));
    connect(transaction, SIGNAL(finished(PackageKit::Transaction::Exit, uint)),
            this, SLOT(removeRepoFinished()));
}


void RpmInstallJob::removeRepoFinished()
{
    qDebug() << "Trying to install" << m_packagePath;

    disconnect(sender(), 0, this, 0);

    PackageKit::Transaction *transaction = new PackageKit::Transaction(this);
    transaction->installFile(m_packagePath, false);

    connect(transaction, SIGNAL(errorCode(PackageKit::Transaction::Error, QString)),
            this, SLOT(errorOccurred(PackageKit::Transaction::Error, QString)));
    connect(transaction, SIGNAL(finished(PackageKit::Transaction::Exit, uint)),
            this, SLOT(installFinished(PackageKit::Transaction::Exit, uint)));
}

void RpmInstallJob::errorOccurred(PackageKit::Transaction::Error error, const QString &message)
{
    qDebug() << "Failed with error:" << error << message;
    setError(Error(Error::Session,
                   QString(QLatin1String("rpm/%1")).arg(error),
                   tr("Install failed"),
                   message));
    setFinished();
}

void RpmInstallJob::installFinished(PackageKit::Transaction::Exit status, uint runtime)
{
    qDebug() << "Job finished, exit code:" << status << "Running time:" << runtime;
    setFinished();
}

}

#include "rpminstalljob.moc"

