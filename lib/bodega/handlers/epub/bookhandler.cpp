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


#include "bookhandler.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtGui/QDesktopServices>

#include "bookinstalljob.h"
#include <assethandlerfactory.h>

using namespace Bodega;

BookHandler::BookHandler(QObject *parent)
    : AssetHandler(parent)
{
    setReady(true);
}

BookHandler::~BookHandler()
{
}

QString BookHandler::filePath() const
{
    QString suffix = QLatin1String("epub");
    if (operations()->mimetype() == QLatin1String("application/pdf")) {
        suffix = QLatin1String("pdf");
    }

    //Te id is to make sure to have an unique filename
    return QDir::homePath() +
           QLatin1String("/Books/") +
           operations()->assetInfo().name + QLatin1String(" - ") + operations()->assetInfo().id +
           QLatin1String(".") + suffix;
}

bool BookHandler::isInstalled() const
{
    QFile f(filePath());
    return f.exists();
}

Bodega::InstallJob *BookHandler::install(QNetworkReply *reply, Session *session)
{
    if (!m_installJob) {
        m_installJob = new BookInstallJob(reply, session, this);
    }

    return m_installJob.data();
}

Bodega::UninstallJob *BookHandler::uninstall(Session *session)
{
    if (!m_uninstallJob) {
        m_uninstallJob = new BookUninstallJob(session, this);
    }

    return m_uninstallJob.data();
}

QString BookHandler::launchText() const
{
    return tr("Read book");
}

void BookHandler::launch()
{
    QDesktopServices::openUrl(QUrl(QLatin1String("file://") + filePath(), QUrl::TolerantMode));
}

class BookHandlerFactory : public Bodega::AssetHandlerFactory
{
public:
    virtual AssetHandler* createHandler() { return new BookHandler; }
};

#include "bookhandler.moc"
Q_EXPORT_PLUGIN2(epubhandler, BookHandlerFactory);

