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

#include "filethread.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>


#ifdef USE_NEPOMUK
#include <nepomuk2/nco.h>
#include <nepomuk2/nfo.h>
#include <nepomuk2/resource.h>
#include <nepomuk2/variant.h>
#endif


// used for moving
FileThread::FileThread(const QString &source, const QString &dest, const QString &author)
    : m_source(source),
      m_dest(dest),
      m_author(author),
      m_move(true)
{

}

// used for deletion
FileThread::FileThread(const QString &pathToDelete)
    : m_source(pathToDelete),
      m_move(false)
{

}

void FileThread::run()
{
    Bodega::Error error;

    if (m_move) {
        QFileInfo info(m_dest);
        const QString path = info.absolutePath();
        QDir dir(path);
        if (!dir.exists()) {
            dir.mkpath(path);
        }

        if (info.exists()) {
            error = Bodega::Error(Bodega::Error::Installation,
                                  QLatin1String("ij/03"),
                                  tr("Install failed"),
                                  tr("A file by this name already exists on the device: %1").arg(info.absoluteFilePath()));
            /**
              While this code creates a new file name successfully ...
              it causes problems when then later trying to hit "Read Book" because
              that code is unaware of the file name change. Needs more infrastructure
            const QString baseName = info.baseName();
            const QString suffix = info.completeSuffix();
            int count = 0;

            while (info.exists()) {
                info.setFile(path + QDir::separator() + baseName + QLatin1Char('(') + QString::number(++count) + QLatin1Char(')') + suffix);
            }

            m_dest = info.absoluteFilePath();
            **/
        } else if (!QFile::rename(m_source, m_dest)) {
            error = Bodega::Error(Bodega::Error::Installation,
                                  QLatin1String("ij/01"),
                                  tr("Install failed"),
                                  tr("Could not install the book; wrong permissions or no space left on device."));

        } else {
#ifdef USE_NEPOMUK
            Nepomuk2::Resource r(m_dest);
            r.addType(Nepomuk2::Vocabulary::NFO::Document());

            //add an author if possuble
            if (!m_author.isEmpty()) {

                Nepomuk2::Resource authorRes = Nepomuk2::Resource();
                authorRes.addType(Nepomuk2::Vocabulary::NCO::Contact());
                authorRes.addProperty(Nepomuk2::Vocabulary::NCO::fullname(), m_author);
                r.addProperty(Nepomuk2::Vocabulary::NCO::creator(), authorRes);
            }
#endif
        }
    } else {
        QFile f(m_source);
        if (!f.exists()) {
            error = Bodega::Error(Bodega::Error::Installation,
                                  QLatin1String("uj/01"),
                                  tr("Uninstall failed"),
                                  tr("The book is not installed."));
        } else if (!f.remove()) {
            error = Bodega::Error(Bodega::Error::Installation,
                                  QLatin1String("uj/02"),
                                  tr("Uninstall failed"),
                                  tr("Unable to delete the file."));
        }
    }

    emit completed(error);
}

#include "filethread.moc"

