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

#ifndef BODEGA_ERROR_H
#define BODEGA_ERROR_H

#include <bodega/globals.h>
#include <QString>

namespace Bodega {

    class BODEGA_EXPORT Error
    {
    public:
        // These should remain in sync with the server implementation
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

        enum Type {
            Network,
            Authentication,
            Parsing,
            Session
        };

        Error();
        Error(const Error &other);
        Error(ServerCode serverCode);
        Error(Type type, const QString &msg);
        Error(Type type,
              const QString &errorId,
              const QString &title,
              const QString &descr);
        ~Error();
        Error &operator=(const Error &rhs);
        ServerCode serverCode() const;
        Type type() const;
        QString errorId() const;
        QString title() const;
        QString description() const;

        static ServerCode serverCodeId(const QString &string);

    private:
        class Private;
        Private * const d;
    };
}

Q_DECLARE_METATYPE(Bodega::Error)

#endif
