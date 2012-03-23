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


#include "assethandler.h"

#include <QDebug>
#include <QDir>
#include <QLibraryInfo>
#include <QPluginLoader>

#include "installjob.h"

namespace Bodega
{

class AssetHandler::Private
{
public:
    Private()
        : ops(0)
    {
    }

    AssetOperations *ops;
};

AssetHandler::AssetHandler(QObject *parent)
    : QObject(parent),
      d(new Private)
{
}

AssetHandler::~AssetHandler()
{
    delete d;
}

void AssetHandler::setOperations(AssetOperations *operations)
{
    d->ops = operations;
}

AssetOperations *AssetHandler::operations() const
{
    return d->ops;
}

QString AssetHandler::launchText() const
{
    return QString();
}

bool AssetHandler::isInstalled() const
{
    return false;
}

Bodega::InstallJob *AssetHandler::install(QNetworkReply *reply, Session *session)
{
    return new InstallJob(reply, session);
}

Bodega::UninstallJob *AssetHandler::uninstall()
{
    //return new UninstallJob();
    return 0;
}

AssetHandler *AssetHandler::create(const QString &type, AssetOperations *parent)
{
    QString plugin;
    //FIXME: a better registration system is badly needed
    if (type == QLatin1String("application/epub+zip") ||
        type == QLatin1String("application/pdf")) {
        plugin = QLatin1String("epub");
    } else if (type == QLatin1String("application/x-plasma")) {
        plugin = QLatin1String("plasmoid");
    } else if (type == QLatin1String("application/x-desktop-wallpaper")) {
        plugin = QLatin1String("wallpaper");
    }

    AssetHandler * handler = 0;

    if (!plugin.isEmpty()) {
        QDir pluginsDir(QLibraryInfo::location(QLibraryInfo::PluginsPath) + QLatin1String("/bodegaassethandlers/"));
        const QString path = pluginsDir.absoluteFilePath(QLatin1String("lib") + plugin + QLatin1String("handlerplugin"));
        QPluginLoader pluginLoader(path);

        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            handler = qobject_cast<Bodega::AssetHandler *>(plugin);
        } else {
            qWarning() << "Plugin load failed" << pluginLoader.errorString();
        }

        if (!handler) {
            delete plugin;
        }
    }

    if (!handler) {
        handler = new AssetHandler;
    }

    handler->setParent(parent);
    handler->setOperations(parent);
    return handler;
}

void AssetHandler::launch()
{
}

} // namespace Bodega

#include "assethandler.moc"

