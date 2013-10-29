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

// used for moving
FileThread::FileThread(const QString &source, const QString &dest)
    : m_source(source),
      m_dest(dest),
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
    //TODO .. error handling?
    if (m_move) {
        QFileInfo info(m_dest);
        QDir dir(info.canonicalPath());
        if (!dir.exists()) {
            dir.mkpath(dir.path());
        }

        QFile::rename(m_source, m_dest);
    } else {
        QFile::remove(m_source);
    }

    emit completed();
}

#include "filethread.moc"

