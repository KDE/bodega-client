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

#include "deletecollectionjob.h"

using namespace Bodega;

class DeletecollectionJob::Private
{
public:
    Private()
    {}

    void init(DeletecollectionJob *q, const QUrl &url);
    DeletecollectionJob *q;
};

void DeletecollectionJob::Private::init(DeletecollectionJob *parent,
                                   const QUrl &url)
{
    q = parent;
}

DeletecollectionJob::DeletecollectionJob(QNetworkReply *reply,
                                 Session *parent)
    : NetworkJob(reply, parent),
      d(new Private)
{
    d->init(this, url());
}

DeletecollectionJob::~DeletecollectionJob()
{
    delete d;
}

void DeletecollectionJob::netFinished(const QVariantMap &result)
{
    parseCommon(result);
}

#include "deletecollectionjob.moc"
