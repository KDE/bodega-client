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

#include "registerjob.h"


#include "session.h"

#include <QDebug>

namespace Bodega
{

class RegisterJob::Private
{
public:
    Private()
    {}
    void init(RegisterJob *parent)
    {
        q = parent;
    }
    RegisterJob *q;
};

RegisterJob::RegisterJob(QNetworkReply *reply,
                         Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this);
}

RegisterJob::~RegisterJob()
{
    delete d;
}

void RegisterJob::netFinished(const QVariantMap &result)
{
     parseCommon(result);
}

}

#include "registerjob.moc"

