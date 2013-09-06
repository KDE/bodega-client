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


#ifndef BODEGA_ASSETHANDLER_H
#define BODEGA_ASSETHANDLER_H

#include <bodega/assetoperations.h>

namespace Bodega
{
    class InstallJob;
    class UninstallJob;

    class BODEGA_EXPORT AssetHandler : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(bool ready READ isReady NOTIFY ready)

        public:
            static AssetHandler *create(const QString &type,
                                        AssetOperations *parent);

            ~AssetHandler();

            bool isReady() const;
            void setReady(bool ready);

            AssetOperations *operations() const;

            //anything accessing operations() will have to be here
            //operations() doesn't exist yet in the constructor
            virtual void init();

            virtual QString launchText() const;
            virtual bool isInstalled() const;

            virtual Bodega::InstallJob *install(QNetworkReply *reply,
                                                Session *session);
            virtual Bodega::UninstallJob *uninstall(Session *session);

            virtual void launch();

        Q_SIGNALS:
            void ready();
            void installedChanged();

        protected:
            AssetHandler(QObject *parent = 0);

        protected Q_SLOTS:
            /**
             * If the asset should be checked for updates, this method
             * should be called after installation is successful
             * 
             * @arg job if a job is passed in, then it is checked for
             *          success before registration happens
             */
            void registerForUpdates(Bodega::NetworkJob *job = 0);

            /**
             * Removes the asset from update checking 
             * on successful uninstallation
             * 
             * @arg job if a job is passed in, then it is checked for
             *          success before registration happens
             */
            void unregisterForUpdates(Bodega::NetworkJob *job = 0);

        private:
            class Private;
            Private * const d;
    };

} // namespace Bodega

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(Bodega::AssetHandler,
                    "com.ct.AssetHandler/1.0");
QT_END_NAMESPACE

#endif

