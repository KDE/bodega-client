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
#include "assethandlerfactory.h"

namespace Bodega
{

class AssetHandler::Private
{
public:
    Private()
        : ops(0),
          ready(false)
    {
    }

    AssetOperations *ops;
    bool ready;
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

void AssetHandler::init()
{
    //Base implementation does nothing
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

Bodega::UninstallJob *AssetHandler::uninstall(Session *session)
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
    } else if (type == QLatin1String("application/x-rpm")) {
        plugin = QLatin1String("rpm");
    }

    AssetHandler * handler = 0;

    if (!plugin.isEmpty()) {
        QList<QByteArray> paths = qgetenv("QT_PLUGIN_PATH").split(':');
        paths.removeAll(QByteArray());
        paths.append(QLibraryInfo::location(QLibraryInfo::PluginsPath).toLocal8Bit());

        foreach (const QByteArray& path, paths) {
            QDir pluginsDir(QString::fromLatin1(path + "/bodegaassethandlers/"));
            //FIXME: the .so is non-portable. need some ifdef's here if this is to work on non-UNIX.
            //remind me why QPluginLoader doesn't do this when QLibrary does?
            const QString candidatePath = pluginsDir.absoluteFilePath(QLatin1String("lib") + plugin + QLatin1String("handlerplugin.so"));
            if(QFile::exists(candidatePath)) {
                QPluginLoader pluginLoader(candidatePath);

                QObject *plugin = pluginLoader.instance();
                if (plugin) {
                    Bodega::AssetHandlerFactory *factory = dynamic_cast<Bodega::AssetHandlerFactory *>(plugin);
                    if (factory) {
                        handler = factory->createHandler();
                        Q_ASSERT(handler);
                    } else {
                        qWarning() << "Could not instanciate the handler using" << candidatePath;
                    }
                    break;
                } else {
                    qWarning() << "Plugin load failed" << pluginLoader.errorString() << candidatePath;
                }
            }
        }
    }

    if (!handler) {
        handler = new AssetHandler;
    }

    handler->setParent(parent);
    handler->d->ops = parent;
    handler->init();
    return handler;
}

void AssetHandler::launch()
{
}

void AssetHandler::registerForUpdates(Bodega::NetworkJob *job)
{
    if (job && job->failed()) {
        return;
    }

    if (!d->ops) {
        return;
    }

    const QString id = d->ops->assetInfo().id;
    if (id.isEmpty()) {
        return;
    }

    //FIXME: store in sql db
    d->initUpdatedb();
}

void AssetHandler::unregisterForUpdates(Bodega::NetworkJob *job)
{
    if (job && job->failed()) {
        return;
    }

    if (!d->ops) {
        return;
    }

    const QString id = d->ops->assetInfo().id;
    if (id.isEmpty()) {
        return;
    }

    //FIXME: delete from sql db
    d->initUpdatedb();
}

void AssetHandler::setReady(bool isReady)
{
    d->ready = isReady;
    if (isReady) {
        emit ready();
    }
}

bool AssetHandler::isReady() const
{
    return d->ready;
}

} // namespace Bodega

#include "assethandler.moc"

