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

namespace Bodega
{

class Session;

/**
 * This class provides a way to hook into the password store
 * and/or client connection service on the system
 */
class CredentialsProvider
{
    public:
        static CredentialsProvider *self();
        virtual ~CredentialsProvider();

        virtual void authenticate(Bodega::Session *session);

    protected:
        CredentialsProvider();

    private:
        class Private;
        Private * const d;
};

} // namespace Bodega
