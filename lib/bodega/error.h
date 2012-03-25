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

#ifndef BODEGA_ERROR_H
#define BODEGA_ERROR_H

#include <bodega/globals.h>
#include <QString>

namespace Bodega {

    class BODEGA_EXPORT Error
    {
    public:
        enum Type {
            Network,
            Authentication,
            Parsing,
            Session
        };
    public:
        Error();
        Error(Type type,
              const QString &msg);
        Error(Type type,
              const QString &errorId,
              const QString &title,
              const QString &descr);
        Type type() const
        {
            return m_type;
        }
        QString errorId() const
        {
            return m_id;
        }
        QString title() const
        {
            return m_title;
        }
        QString description() const
        {
            return m_description;
        }

    private:
        Type m_type;
        QString m_id;
        QString m_title;
        QString m_description;
    };
}

Q_DECLARE_METATYPE(Bodega::Error)

#endif
