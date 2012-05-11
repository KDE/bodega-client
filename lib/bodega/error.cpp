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

#include "error.h"

#include <QObject>

using namespace Bodega;

class Error::Private
{
public:
    Private(Type t,
            const QString &errorId,
            const QString &title,
            const QString &descr)
      : code(NoCode),
        type(t),
        id(errorId),
        title(title),
        description(descr)
    {
    }


    Private(ServerCode c)
        : code(NoCode)
    {
        setServerCode(c);
    }

    Private(Type t)
        : code(NoCode),
          type(t)
    {
    }

    void setServerCode(ServerCode c);
    static void initServerCodeDict();

    ServerCode code;
    Type type;
    QString id;
    QString title;
    QString description;

    static QMap<QString, ServerCode> serverCodes;
};

QMap<QString, Error::ServerCode> Error::Private::serverCodes;

void Error::Private::initServerCodeDict()
{
    if (!serverCodes.isEmpty()) {
        return;
    }
    #define ADD_SERVER_CODE(CODE) serverCodes.insert(QLatin1String("" #CODE), CODE);
    ADD_SERVER_CODE(NoCode)
    ADD_SERVER_CODE(Connection)
    ADD_SERVER_CODE(Unknown)
    ADD_SERVER_CODE(Database)
    ADD_SERVER_CODE(Unauthorized)
    ADD_SERVER_CODE(MissingParameters)
    ADD_SERVER_CODE(NoMatch)
    ADD_SERVER_CODE(AccountInactive)
    ADD_SERVER_CODE(AccountExists)
    ADD_SERVER_CODE(PurchaseFailed)
    ADD_SERVER_CODE(MailerFailure)
    ADD_SERVER_CODE(AccountUpdateFailed)
    ADD_SERVER_CODE(EncryptionFailure)
    ADD_SERVER_CODE(PasswordTooShort)
    ADD_SERVER_CODE(Download)
    ADD_SERVER_CODE(AccessDenied)
    ADD_SERVER_CODE(RedeemCodeFailure)

    ADD_SERVER_CODE(PaymentMethodCreation)
    ADD_SERVER_CODE(PaymentMethodDeletion)
    ADD_SERVER_CODE(PaymentMethodDetails)
    ADD_SERVER_CODE(PaymentMethodMissing)
    ADD_SERVER_CODE(CustomerRetrieval)
    ADD_SERVER_CODE(CustomerDatabase)
    ADD_SERVER_CODE(CustomerCharge)
    ADD_SERVER_CODE(CustomerRefund)
    ADD_SERVER_CODE(ChargeNotRecorded)
    ADD_SERVER_CODE(NotEnoughPoints)
    ADD_SERVER_CODE(TooManyPoints)
    ADD_SERVER_CODE(CollectionExists)
    ADD_SERVER_CODE(AssetExists)
    ADD_SERVER_CODE(CardDeclined)
    ADD_SERVER_CODE(CardIncorrectNumber)
    ADD_SERVER_CODE(CardInvalidNumber)
    ADD_SERVER_CODE(CardInvalidExpiryMonth)
    ADD_SERVER_CODE(CardInvalidExpiryYear)
    ADD_SERVER_CODE(CardInvalidCVC)
    ADD_SERVER_CODE(CardExpired)
    ADD_SERVER_CODE(CardInvalidAmount)
    ADD_SERVER_CODE(CardDuplicateTransaction)
    ADD_SERVER_CODE(CardProcessingError)
    ADD_SERVER_CODE(PurchaseNotEnoughPoints)
    ADD_SERVER_CODE(PurchaseTooManyPoints)
}

void Error::Private::setServerCode(ServerCode c)
{
    code = c;
    type = Session;
    id = QLatin1String("session/") + QString::number(c);
    title = QObject::tr("Session Error");

    switch (code) {
        case Connection:
            description = QObject::tr("Connection failed.");
        break;
        case Database:
            description = QObject::tr("The add-ons database could not be reached.");
        break;
        case Unauthorized:
            description = QObject::tr("This account may not perform the requested action.");
        break;
        case MissingParameters:
            description = QObject::tr("Required information was missing from the request.");
        break;
        case NoMatch:
            description = QObject::tr("No matching records found.");
        break;
        case AccountInactive:
            description = QObject::tr("This account is not active.");
        break;
        case AccountExists:
            description = QObject::tr("An account under that name already exists.");
        break;
        case PurchaseFailed:
            description = QObject::tr("Could not purchase the requested item.");
        break;
        case MailerFailure:
            description = QObject::tr("Failed to send email.");
        break;
        case AccountUpdateFailed:
            description = QObject::tr("Failed to change account information.");
        break;
        case EncryptionFailure:
            description = QObject::tr("Encryption failed, please try again.");
        break;
        case PasswordTooShort:
            description = QObject::tr("The password is too short, please try again.");
        break;
        case Download:
            description = QObject::tr("Could not complete the download of the requested item.");
        break;
        case AccessDenied:
            description = QObject::tr("Could not complete the download of the requested item.");
        break;
        case RedeemCodeFailure:
            description = QObject::tr("The supplied points code was not valid.");
        break;

        case PaymentMethodCreation:
            title = QObject::tr("Payment Method Error");
            description = QObject::tr("Failed to update purchase method details.");
        break;
        case PaymentMethodDeletion:
            title = QObject::tr("Payment Method Error");
            description = QObject::tr("Failed to update purchase method details.");
        break;
        case PaymentMethodDetails:
            title = QObject::tr("Payment Method Error");
            description = QObject::tr("Failed to update purchase method details.");
        break;
        case PaymentMethodMissing:
            title = QObject::tr("Payment Method Error");
            description = QObject::tr("A valid payment method could not be found. Please add a payment method to your account.");
        break;
        case CustomerRetrieval:
            title = QObject::tr("Purchase Method Error");
            description = QObject::tr("No purchase method (e.g. credit card) information found.");
        break;
        case CustomerDatabase:
            description = QObject::tr("Could not locate the requested customer account.");
        break;
        case CustomerCharge:
        case CustomerRefund:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("Unable to complete the purchase process. No charge was made to your card.");
        break;
        case ChargeNotRecorded:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("Transaction was successful, but could not record the result on the account");
        break;
        case NotEnoughPoints:
            title = QObject::tr("Purchase Error");
            description = QObject::tr("Your account lacks enough points to make this purchase.");
        break;
        case TooManyPoints:
            title = QObject::tr("Purchase Error");
            description = QObject::tr("The requested amount of points exceeds the maximum purchase limit.");
        break;

        case CollectionExists:
            title = QObject::tr("Creation Error");
            description = QObject::tr("The collection already exists.");
        break;
        case AssetExists:
            title = QObject::tr("Creation Error");
            description = QObject::tr("The asset already exists.");
        break;

        case CardDeclined:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("Your card has been declined. Please try another payment method");
        break;

        case CardIncorrectNumber:
        case CardInvalidNumber:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("The card number supplied is incorrect. Please check it and try again.");
        break;

        case CardInvalidExpiryMonth:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("The card expiry month supplied is incorrect.");
        break;

        case CardInvalidExpiryYear:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("The card expiry year supplied is incorrect.");
        break;

        case CardInvalidCVC:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("The card CVC supplied is incorrect.");
        break;

        case CardExpired:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("Your card has expired. Please try another payment method.");
        break;

        case CardInvalidAmount:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("The purchase amount was invalid. Please try again.");
        break;

        case CardDuplicateTransaction:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("This transaction was already processed.");
        break;

        case CardProcessingError:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("A processing error occured when making the purchase. Please try again.");
        break;

        case PurchaseNotEnoughPoints:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("Too few points were requested. A minimum of 500 points per purchase is required.");
        break;

        case PurchaseTooManyPoints:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("Too many points were requested. Please purchase fewer points.");
        break;

        // NoCode, Unknown or out of bounds errors
        default:
            description = QObject::tr("An unknown error occurred.");
        break;
    }
}

Error::Error()
    : d(new Private(Session))
{
}

Error::Error(const Error &other)
    : d(new Private(*other.d))
{

}

Error::Error(ServerCode code)
    : d(new Private(code))
{
}

Error::Error(Type type, const QString &msg)
    : d(new Private(type, msg, msg, msg))
{
}

Error::Error(Type type,
             const QString &errorId,
             const QString &title,
             const QString &descr)
    : d(new Private(type, errorId, title, descr))
{
}

Error::~Error()
{
    delete d;
}

Error &Error::operator=(const Error &rhs)
{
    d->code = rhs.d->code;
    d->type = rhs.d->type;
    d->id = rhs.d->id;
    d->title = rhs.d->title;
    d->description = rhs.d->description;
    return *this;
}

Error::ServerCode Error::serverCode() const
{
    return d->code;
}

Error::Type Error::type() const
{
    return d->type;
}

QString Error::errorId() const
{
    return d->id;
}

QString Error::title() const
{
    return d->title;
}

QString Error::description() const
{
    return d->description;
}

Error::ServerCode Error::serverCodeId(const QString &string)
{
    Private::initServerCodeDict();
    if (Private::serverCodes.contains(string)) {
        return Private::serverCodes.value(string);
    }

    return Unknown;
}

