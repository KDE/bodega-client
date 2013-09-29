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

#include <updater.h>

#include <QFile>
#include <QMetaObject>
#include <QProcess>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTimer>

#include <KConfigGroup>
#include <KDebug>
#include <KDesktopFile>
#include <KDirWatch>
#include <KLocale>
#include <KPluginFactory>
#include <KRun>
#include <KStandardDirs>
#include <KStatusNotifierItem>
#include <KToolInvocation>

#include <Solid/Networking>

#include "bodega/assethandler.h"
#include "bodega/session.h"
#include "bodega/updatescheckjob.h"

const char *defaultPackage = "org.kde.desktop";

K_PLUGIN_FACTORY(UpdaterFactory, registerPlugin<Updater>();)
K_EXPORT_PLUGIN(UpdaterFactory("bodegaupdater"))

const int checkInterval = 6 * 60 * 60; // 6 hours

Updater::Updater(QObject *parent, const QVariantList &)
    : KDEDModule(parent),
      m_dbPath(Bodega::AssetHandler::updateDatabasePath()),
      m_checkTimer(new QTimer(this)),
      m_notifier(0)
{
    m_checkTimer->setInterval(checkInterval * 1000);
    connect(m_checkTimer, SIGNAL(timeout()), this, SLOT(checkForUpdates()));

    connect(KDirWatch::self(), SIGNAL(created(QString)), this, SLOT(fileDirtied(QString)));
    connect(KDirWatch::self(), SIGNAL(deleted(QString)), this, SLOT(fileDirtied(QString)));
    KDirWatch::self()->addFile(m_dbPath);

    connect(Solid::Networking::notifier(), SIGNAL(statusChanged(Solid::Networking::Status)),
            this, SLOT(networkStatusChanged(Solid::Networking::Status)));

    if (QFile::exists(m_dbPath)) {
        QMetaObject::invokeMethod(this, "initDb", Qt::QueuedConnection);
    }
}

void Updater::updateNotifier()
{
    int updates = 0;
    if (m_db.isValid()) {
        QSqlQuery query;
        if (query.exec("SELECT count(updated) FROM assets WHERE updated")) {
            query.first();
            updates = query.value(0).toInt();
        } else {
            kDebug() << "QUERY FAILED" << query.lastError();
        }
    }

    if (updates > 0) {
        m_notifier->setStatus(KStatusNotifierItem::Active);
        m_notifier->setToolTip("system-software-update", i18n("Updates available!"),
                               i18np("There is one update available for installed add-on content",
                                     "There are %1 updates available for installed add-on content",
                                     updates));
    } else {
        m_notifier->setStatus(KStatusNotifierItem::Passive);
        m_notifier->setToolTip("system-software-update", i18n("All content updated"),
                               i18n("All of your installed add-on content is up to date"));
    }
}

void Updater::fileDirtied(const QString &file)
{
    if (file == m_dbPath) {
        if (m_db.isValid()) {
            m_db.close();
        }

        initDb();
    }
}

void Updater::networkStatusChanged(Solid::Networking::Status status)
{
    switch (status) {
        case Solid::Networking::Connected:
            checkForUpdates();
            m_checkTimer->start();
            break;
        default:
            m_checkTimer->stop();
        break;
    }
}

void Updater::checkForUpdates()
{
    if (!m_db.isValid()) {
        return;
    }

    QSqlQuery query(m_db);

    if (!query.exec("select strftime('%s', 'now') - val from config timestamps where key = 'lastcheck'")) {
        kDebug() << "Failed to lookup time in database" << query.lastError();
        return;
    }

    if (!query.first()) {
        query.exec(QLatin1String("INSERT INTO config (key, val) VALUES('lastcheck', strftime('%s', 'now'))"));
    } else if (query.value(0).toInt() < checkInterval) {
        // we've already checked recently; maybe we got re-started or something similar
        return;
    } else {
        if (!query.exec("UPDATE config SET val = strftime('%s', 'now') WHERE key = 'lastcheck'")) {
            kDebug() << "Failed to update strtime...." << query.lastError();
        }
    }

    if (!query.exec("SELECT rowid, store, warehouse, asset, created FROM assets WHERE NOT checked ORDER BY warehouse, store LIMIT 100")) {
        kDebug() << "Failed to query the Bodega update database" << query.lastError();
        return;
    }

    // loop through results .. fire off a request for each store / warehouse pair
    QString currentStore;
    QString currentWarehouse;
    QSqlRecord record = query.record();
    const int storeCol = record.indexOf(QLatin1String("store"));
    const int warehouseCol = record.indexOf(QLatin1String("warehouse"));
    const int assetCol = record.indexOf(QLatin1String("asset"));
    const int createdCol = record.indexOf(QLatin1String("created"));
    const int rowidCol= record.indexOf(QLatin1String("rowid"));
    QList<QPair<QString, QString> > assets;
    QStringList rowIds;
    while (query.next()) {
        const QString store = query.value(storeCol).toString();
        const QString warehouse = query.value(warehouseCol).toString();
        const QString asset = query.value(assetCol).toString();
        const QString created = query.value(createdCol).toString();
        if (currentStore != store || currentWarehouse != warehouse) {
            if (!currentStore.isEmpty()) {
                sendUpdatesCheck(currentStore, currentWarehouse, assets);
                assets.clear();
            }

            currentStore = store;
            currentWarehouse = warehouse;
        }

        assets.append(qMakePair<QString, QString>(asset, created));
        rowIds.append(QString::number(query.value(rowidCol).value<qint64>()));
    }

    if (currentStore.isEmpty()) {
        // we got nothing back, which everything has been checked! so let's set everything to be
        // checked again
        if (!query.exec("UPDATE assets SET checked = 0")) {
            kDebug() << "Failed to update assets with checked status:" << query.lastQuery();
        }
    } else {
        if (!query.exec(QString("UPDATE assets SET updated = 0, checked = 1 WHERE rowid IN (%1)").arg(rowIds.join(", ")))) {
            kDebug() << "Failed to update assets with checked status:" << query.lastQuery();
        }
        sendUpdatesCheck(currentStore, currentWarehouse, assets);
    }
}

void Updater::sendUpdatesCheck(const QString &store, const QString &warehouse, const QList<QPair<QString, QString> > &assets)
{
    const QString sessionKey(store + '_' + warehouse);
    Bodega::Session *session = m_sessions.value(sessionKey);
    if (!session) {
        session = new Bodega::Session(this);
        session->setBaseUrl(warehouse);
        session->setStoreId(store);
        m_sessions.insert(sessionKey, session);
    }

    Bodega::UpdatesCheckJob *job = session->updatesCheck(assets);
    connect(job, SIGNAL(jobFinished(Bodega::NetworkJob*)), this, SLOT(updatesCheckFinished(Bodega::NetworkJob*)));
}

void Updater::updatesCheckFinished(Bodega::NetworkJob *job)
{
    if (!m_db.isValid()) {
        kDebug() << "Got an update job, but the database is invalid";
        return;
    }

    Bodega::UpdatesCheckJob *updateJob = qobject_cast<Bodega::UpdatesCheckJob *>(job);
    if (!updateJob) {
        kDebug() << "We were sent a non-update job .. that's wrong.";
        return;
    }

    if (job->failed()) {
        Bodega::Error e = job->error();
        kDebug() << "Job failed: " << e.serverCode() << e.type() << e.errorId() << e.title() << e.description();
        return;
    }

    if (updateJob->updatedAssets().isEmpty()) {
        return;
    }

    QSqlQuery query(m_db);
    // we don't use proper binding here because the binder messes up when asked to bind something in ()s
    query.prepare("UPDATE assets SET updated = 1 WHERE asset IN (" + updateJob->updatedAssets().join(",") + ") AND warehouse = :warehouse AND store = :store");
    query.bindValue(":warehouse", updateJob->session()->baseUrl());
    query.bindValue(":store", updateJob->session()->storeId());
    if (!query.exec()) {
        kDebug() << "Failed to mark updated assets in the database:" << query.lastError();
    }
    updateNotifier();
}

void Updater::initDb()
{
    m_db = Bodega::AssetHandler::updateDatabase();

    if (m_db.isValid()) {
        if (!m_notifier) {
            m_notifier = new KStatusNotifierItem(this);
            m_notifier->setStatus(KStatusNotifierItem::Passive);
            m_notifier->setCategory(KStatusNotifierItem::SystemServices);
            m_notifier->setTitle(i18n("Add-ons Updater"));
            //TODO: get a bodega / add-ons icon?
            m_notifier->setIconByName("system-software-update");
            connect(m_notifier, SIGNAL(activateRequested(bool,QPoint)),
                    this, SLOT(showUpdateUi()));
        }

        updateNotifier();
        networkStatusChanged(Solid::Networking::status());
    } else {
        delete m_notifier;
        m_notifier = 0;
        m_checkTimer->stop();
    }
}

void Updater::showUpdateUi()
{
    if (m_notifier && m_notifier->status() == KStatusNotifierItem::Active) {
        QString updaterPath = KStandardDirs::locate("xdgdata-apps", "kde4/active-addons-updater.desktop");
        KDesktopFile desktopFile(updaterPath);
        updaterPath = desktopFile.desktopGroup().readEntry("Exec", QString());
        if (!updaterPath.isEmpty()) {
            QProcess::startDetached(updaterPath);
            //QStringList args = KShell::splitArgs(updaterPath);
            //KRun::run(updaterPath, KUrl::List(), 0);
        }
    }
}

#include "updater.moc"
