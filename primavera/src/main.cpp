/***************************************************************************
 *                                                                         *
 *   Copyright 2012 Coherent Theory LLC                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

// KDE
#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KDE/KLocale>

#include "kdeclarativemainwindow.h"
#include "kdeclarativeview.h"

static const char version[] = "0.1";

int main(int argc, char **argv)
{
    KAboutData about("bodega-store", 0, ki18n("Add Ons App"), version, ki18n(""),
                     KAboutData::License_GPL, ki18n("Copyright 2012 Coherent Theory LLC"),
                     KLocalizedString(), 0, "vivaldi@makeplaylive.com");
    about.addAuthor( ki18n("Aaron Seigo"), KLocalizedString(), "aseigo@makeplaylive.com" ),

    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    //kDebug() << "ARGS:" << args << args->count();

    KDeclarativeMainWindow *window = new KDeclarativeMainWindow;
    window->declarativeView()->setPackageName("com.coherenttheory.primavera");
    window->show();
    args->clear();
    return app.exec();
}
