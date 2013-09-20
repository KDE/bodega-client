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

#ifndef UPDATER_H
#define UPDATER_H

#include <KDEDModule>

#include <Solid/Networking>

#include <QHash>
#include <QPair>
#include <QSqlDatabase>
#include <QVariantList>

class QTimer;

class KStatusNotifierItem;

namespace Bodega
{
    class Session;
    class NetworkJob;
} // namespace Bodega

class Updater : public KDEDModule
{
    Q_OBJECT

public:
    Updater(QObject *parent = 0, const QVariantList &args = QVariantList());

private Q_SLOTS:
    void fileDirtied(const QString &file);
    void networkStatusChanged(Solid::Networking::Status status);
    void checkForUpdates();
    void updateCheckFinished(Bodega::NetworkJob *job);
    void initDb();

private:
    void sendUpdateCheck(const QString &store, const QString &warehouse, const QList<QPair<QString, QString> > &assets);
    void updateNotifier();

    QString m_dbPath;
    QSqlDatabase m_db;
    QTimer *m_checkTimer;
    QHash<QString, Bodega::Session *> m_sessions;
    KStatusNotifierItem *m_notifier;
};

#endif

