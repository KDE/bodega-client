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

#include "error.h"

using namespace Bodega;

Error::Error()
    : m_type(Error::Session)
{
}

Error::Error(Type type,
             const QString &msg)
    : m_type(type),
      m_id(msg),
      m_title(msg),
      m_description(msg)
{
}

Error::Error(Type type,
             const QString &errorId,
             const QString &title,
             const QString &descr)
    : m_type(type),
      m_id(errorId),
      m_title(title),
      m_description(descr)
{
}
