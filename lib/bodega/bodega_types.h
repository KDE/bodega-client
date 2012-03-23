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

#ifndef BODEGA_TYPES_H
#define BODEGA_TYPES_H

#include <QDateTime>
#include <QString>
#include <QByteArray>

struct Mimetypes
{
    int id;
    QString type;
};

struct Licenses
{
    int id;
    QString name;
    QString text;
};

struct PersonRoles
{
    int id;
    QString description;
};

struct Affiliations
{
    int person;
    int partner;
    int role;
};

struct Devices
{
    int id;
    int partner;
    QString partNumber;
};

struct Sessions
{
    int person;
    int device;
    QDateTime stamp;
};

struct Assets
{
    int id;
    int mimetype;
    int license;
    int author;
    QString externId;
    QString name;
    QString version;
};

struct ChannelAssets
{
    int channel;
    int asset;
};

struct Languages
{
    int id;
    QByteArray code;
    QString name;
};

struct Assettext
{
    int asset;
    QByteArray language;
};

struct Channels
{
    int id;
    int partner;
    int parent;
    QString name;
    bool active;
};

struct DeviceChannels
{
    int channel;
    int device;
};

struct ChannelText
{
    int channel;
    QByteArray language;
    QString name;
};

struct TagTypes
{
    int id;
    QString type;
};

struct Tags
{
    int id;
    int partner;
    int type;
    QString title;
};

struct AssetTags
{
    int asset;
    int tag;
};

struct ChannelTags
{
    int channel;
    int tag;
};

struct TagText
{
    int tag;
    QByteArray language;
    QString title;
};


#endif /* BODEGA_TYPES_H */
