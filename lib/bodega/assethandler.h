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
        public:
            AssetHandler(QObject *parent = 0);
            ~AssetHandler();

            void setOperations(AssetOperations *operations);
            AssetOperations *operations() const;

            virtual QString launchText() const;
            virtual bool isInstalled() const;

            virtual Bodega::InstallJob *install(QNetworkReply *reply,
                                                Session *session);
            virtual Bodega::UninstallJob *uninstall(Session *session);

            virtual void launch();

            static AssetHandler *create(const QString &type,
                                        AssetOperations *parent);

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

