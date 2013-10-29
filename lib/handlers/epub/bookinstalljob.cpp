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


#include "bookinstalljob.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include "assetoperations.h"
#include "bookhandler.h"

#ifdef USE_NEPOMUK
#include <nepomuk2/nco.h>
#include <nepomuk2/nfo.h>
#include <nepomuk2/resource.h>
#include <nepomuk2/variant.h>
#endif

namespace Bodega
{

BookInstallJob::BookInstallJob(QNetworkReply *reply, Session *session,
                               BookHandler *handler)
    : InstallJob(reply, session)
{
    m_handler = handler;
}

BookInstallJob::~BookInstallJob()
{
}

void BookInstallJob::downloadFinished(const QString &localFile)
{
    QFile f(localFile);

    QDir dir(QDir::homePath() + QString::fromLatin1("/Books/"));

    if (!dir.exists()) {
        dir.mkdir(dir.path());
    }

    if (!f.rename(dir.filePath(m_handler->filePath()))) {
        setError(Error(Error::Session,
                       QLatin1String("ij/01"),
                       tr("Install failed"),
                       tr("Impossible to install the book, wrong permissions or no space left on device.")));
    } else {
#ifdef USE_NEPOMUK
        Nepomuk2::Resource r(dir.filePath(m_handler->filePath()));
        r.addType(Nepomuk2::Vocabulary::NFO::Document());

        //add an author if possuble
        if (m_handler->operations()->assetTags().contains(QLatin1String("author"))) {

            Nepomuk2::Resource authorRes = Nepomuk2::Resource();
            authorRes.addType(Nepomuk2::Vocabulary::NCO::Contact());
            authorRes.addProperty(Nepomuk2::Vocabulary::NCO::fullname(), m_handler->operations()->assetTags().value(QLatin1String("author")));
            r.addProperty(Nepomuk2::Vocabulary::NCO::creator(), authorRes);
        }
#endif
    }
    setFinished();
}

}

#include "bookinstalljob.moc"

