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

#ifndef BODEGA_FILETHREAD
#define BODEGA_FILETHREAD

#include <QObject>
#include <QRunnable>

#include "bodega/error.h"

class FileThread : public QObject, public QRunnable
{
    Q_OBJECT

public:
    // used for moving 
    FileThread(const QString &source, const QString &dest, const QString &m_author);

    // used for deletion
    FileThread(const QString &pathToDelete);

    void run();

Q_SIGNALS:
    void completed(const Bodega::Error &error);

private:
    QString m_source;
    QString m_dest;
    QString m_author;
    bool m_move;
};

#endif

