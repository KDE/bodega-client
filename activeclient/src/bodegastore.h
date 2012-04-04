/***************************************************************************
 *                                                                         *
 *   Copyright 2012 Marco Martin <mart@kde.org>                            *
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


#ifndef BODEGASTORE_H
#define BODEGASTORE_H

#include "kdeclarativemainwindow.h"

namespace Bodega {
    class HistoryModel;
    class Model;
    class Session;
}

Q_DECLARE_METATYPE(Bodega::Session*)

class BodegaStore : public KDeclarativeMainWindow
{
    Q_OBJECT
    Q_PROPERTY(Bodega::Session *session READ session CONSTANT)
    Q_PROPERTY(Bodega::Model *channelsModel READ channelsModel CONSTANT)
    Q_PROPERTY(Bodega::Model *searchModel READ searchModel CONSTANT)
    Q_PROPERTY(Bodega::HistoryModel *historyModel READ historyModel CONSTANT)

public:
    BodegaStore();
    virtual ~BodegaStore();


    Bodega::Session *session() const;
    Bodega::Model* channelsModel() const;
    Bodega::Model* searchModel() const;
    Bodega::HistoryModel *historyModel();

    Q_INVOKABLE void saveCredentials() const;
    Q_INVOKABLE QVariantHash retrieveCredentials() const;
    Q_INVOKABLE void historyInUse(bool used);

private:
    Bodega::Session *m_session;
    Bodega::Model *m_channelsModel;
    Bodega::Model *m_searchModel;
    Bodega::HistoryModel *m_historyModel;
    int m_historyUsers;
};

#endif // BODEGASTORE_H
