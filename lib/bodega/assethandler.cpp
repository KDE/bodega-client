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
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

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

    void initUpdateDb() {
        if (updateDb.isOpen()) {
            return;
        }

        updateDb = AssetHandler::updateDatabase();
    }

    AssetOperations *ops;
    bool ready;
    QSqlDatabase updateDb;
};

AssetHandler::AssetHandler(QObject *parent)
    : QObject(parent),
      d(new Private)
{
}

AssetHandler::~AssetHandler()
{
    if (d->updateDb.isOpen()) {
        d->updateDb.close();
    }

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
    if (type.startsWith(QLatin1String("application/epub")) ||
        type == QLatin1String("application/pdf")) {
        plugin = QLatin1String("epub");
    } else if (type == QLatin1String("application/x-plasma")) {
        plugin = QLatin1String("plasmoid");
    } else if (type == QLatin1String("application/x-desktop-wallpaper") ||
               type == QLatin1String("image/png") ||
               type == QLatin1String("image/jpeg")) {
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
    if (!job || job->failed()) {
        return;
    }

    if (!d->ops) {
        return;
    }

    const QString id = d->ops->assetInfo().id;
    if (id.isEmpty()) {
        return;
    }

    d->initUpdateDb();

    QSqlQuery query(d->updateDb);

    query.prepare(QLatin1String("SELECT asset FROM assets WHERE warehouse = :warehouse AND store = :store AND asset = :asset"));
    query.bindValue(QLatin1String(":warehouse"), job->session()->baseUrl());
    query.bindValue(QLatin1String(":store"), job->session()->storeId());
    query.bindValue(QLatin1String(":asset"), id);

    if (query.exec() && query.first()) {
        query.prepare(QLatin1String("UPDATE assets SET create = :created, updated = 0, checked = 0 WHERE warehouse = :warehouse AND store = :store AND asset = :asset"));
    } else {
        query.prepare(QLatin1String("INSERT INTO assets (warehouse, store, asset, created) VALUES (:warehouse, :store, :asset, :created)"));
    }

    query.bindValue(QLatin1String(":warehouse"), job->session()->baseUrl());
    query.bindValue(QLatin1String(":store"), job->session()->storeId());
    query.bindValue(QLatin1String(":asset"), id);
    query.bindValue(QLatin1String(":created"), d->ops->assetInfo().created.toString(Qt::ISODate));
    if (!query.exec()) {
        qDebug() << "Insertion of update failed:" << query.lastError();
    }
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

    d->initUpdateDb();

    QSqlQuery query(d->updateDb);
    query.prepare(QLatin1String("DELETE FROM assets WHERE warehouse = :warehouse AND store = :store AND asset = :asset"));
    query.bindValue(QLatin1String(":warehouse"), job->session()->baseUrl());
    query.bindValue(QLatin1String(":store"), job->session()->storeId());
    query.bindValue(QLatin1String(":asset"), id);
    if (!query.exec()) {
        qDebug() << "Insertion of update failed:" << query.lastError();
    }
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

QString AssetHandler::updateDatabasePath()
{
    //FIXME QT5: use QStandardDirs for this
    QString updateDbPath = QDir::homePath() + QLatin1String("/.local/share/bodega/");
    if (!QFile::exists(updateDbPath)) {
        QDir dir;
        dir.mkpath(updateDbPath);
    }

    updateDbPath.append(QLatin1String("assets.db"));
    return updateDbPath;
}

QSqlDatabase AssetHandler::updateDatabase()
{
    const QString dbConnectionName = QLatin1String("BODEGA_UPDATES");
    bool initTables = false;

    QSqlDatabase updateDb = QSqlDatabase::database(dbConnectionName);

    if (!updateDb.isValid()) {
        const QString updateDbPath = updateDatabasePath();
        initTables = !QFile::exists(updateDbPath);
        updateDb = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
        updateDb.setDatabaseName(updateDbPath);
    }

    if (!updateDb.isOpen()) {
        if (!updateDb.open()) {
            qDebug() << "failed to open update db" << updateDb.lastError();
        }

        if (initTables) {
            QSqlQuery query(updateDb);
            query.exec(QLatin1String("CREATE TABLE assets (store text, warehouse text, asset text, created text, updated bool default false, checked bool default false)"));
            query.exec(QLatin1String("CREATE TABLE config (key text, val int)"));
            query.exec(QLatin1String("INSERT INTO config (key, val) VALUES('lastcheck', strftime('%s', 'now'))"));
            query.exec(QLatin1String("INSERT INTO config (key, val) VALUES('version', 1)"));
        }
    }

    return updateDb;
}

} // namespace Bodega

#include "assethandler.moc"

