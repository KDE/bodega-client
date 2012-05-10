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

class ErrorCode : public QObject
{
    Q_OBJECT
public:
    // These should remain in sync with the library and the server implementation
    enum ServerCode {
        NoCode                = 0,
        Connection            = 1, //reserved for client side connection errors
        Unknown               = 2, //unexpected problem
        Database              = 3, //db communication problem
        Unauthorized          = 4, //unauthorized access
        MissingParameters     = 5, //username/password/device id missing
        NoMatch               = 6, //no or incorrect result
        AccountInactive       = 7, //account is inactive
        AccountExists         = 8, //collection/asset already exists
        PurchaseFailed        = 9, //failed to purchase item
        MailerFailure         = 10, //failed to generate an email
        AccountUpdateFailed   = 11, //account update failed
        EncryptionFailure     = 12, //couldn't encrypt text
        PasswordTooShort      = 13, //password has to be at least 8 characters
        Download              = 14, //problem with the download
        AccessDenied          = 15, //access denied to the requested asset
        RedeemCodeFailure     = 16, //could not redeem points code.

        PaymentMethodCreation = 17, //could not link credit card to customer
        PaymentMethodDeletion = 18, //delete card details
        PaymentMethodDetails  = 19, //problem updating customer card details
        PaymentMethodMissing  = 20, //customer card details missing
        CustomerRetrieval     = 21, //problems retrieving card details
        CustomerDatabase      = 22, //db error while querying for customer id
        CustomerCharge        = 23, //unable to charge the customers card
        CustomerRefund        = 24, //unable to charge the customers card
        ChargeNotRecorded     = 25, //charged the customer but couldn't record it
        NotEnoughPoints       = 26, //not enough points for a purchase
        TooManyPoints         = 27,  //tried to buy too many points at once
        CollectionExists      = 28, //collection already exists
        AssetExists           = 29  //asset already exists
    };
    Q_ENUMS(ServerCode)
};

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
    Q_INVOKABLE void forgetCredentials() const;
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
