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

#include "credentialsprovider.h"

namespace Bodega
{

class CredentialsProvider::Private
{
public:
    static CredentialsProvider *self;
};

CredentialsProvider *CredentialsProvider::Private::self = 0;

CredentialsProvider::CredentialsProvider()
    : d(0)
{
    Private::self = this;
}

CredentialsProvider *CredentialsProvider::self()
{
    if (!Private::self) {
        new CredentialsProvider();
    }

    return Private::self;
}

CredentialsProvider::~CredentialsProvider()
{
}

void CredentialsProvider::authenticate(Bodega::Session *)
{
}

} // namespace Bodega
