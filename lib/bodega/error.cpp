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

#include <QMetaEnum>
#include <QObject>

using namespace Bodega;

class Error::Private
{
public:
    Private(Type t,
            const QString &errorId,
            const QString &title,
            const QString &descr)
      : code(ErrorCodes::NoCode),
        type(t),
        id(errorId),
        title(title),
        description(descr)
    {
    }


    Private(ErrorCodes::ServerCode c)
        : code(ErrorCodes::NoCode)
    {
        setServerCode(c);
    }

    Private(Type t)
        : code(ErrorCodes::NoCode),
          type(t)
    {
    }

    void setServerCode(ErrorCodes::ServerCode c);
    static void initServerCodeDict();

    ErrorCodes::ServerCode code;
    Type type;
    QString id;
    QString title;
    QString description;

    static QMap<QString, ErrorCodes::ServerCode> serverCodes;
};

QMap<QString, ErrorCodes::ServerCode> Error::Private::serverCodes;

void Error::Private::initServerCodeDict()
{
    if (!serverCodes.isEmpty()) {
        return;
    }

    ErrorCodes codes;
    QMetaEnum e = codes.metaObject()->enumerator(codes.metaObject()->indexOfEnumerator("ServerCode"));
    for (int i = 0; i < e.keyCount(); ++i) {
        serverCodes.insert(QLatin1String(e.key(i)), static_cast<ErrorCodes::ServerCode>(e.value(i)));
    }
}

void Error::Private::setServerCode(ErrorCodes::ServerCode c)
{
    code = c;
    type = Session;
    id = QLatin1String("session/") + QString::number(c);
    title = QObject::tr("Session Error");

    switch (code) {
        case ErrorCodes::Connection:
            description = QObject::tr("Connection failed.");
        break;
        case ErrorCodes::Database:
            description = QObject::tr("The add-ons database could not be reached.");
        break;
        case ErrorCodes::Unauthorized:
            description = QObject::tr("This account may not perform the requested action.");
        break;
        case ErrorCodes::MissingParameters:
            description = QObject::tr("Required information was missing from the request.");
        break;
        case ErrorCodes::NoMatch:
            description = QObject::tr("Wrong user name or password.");
        break;
        case ErrorCodes::AccountInactive:
            description = QObject::tr("This account is not active.");
        break;
        case ErrorCodes::AccountExists:
            description = QObject::tr("An account for that email already exists.");
        break;
        case ErrorCodes::PurchaseFailed:
            description = QObject::tr("Could not purchase the requested item.");
        break;
        case ErrorCodes::MailerFailure:
            description = QObject::tr("Failed to send email.");
        break;
        case ErrorCodes::AccountUpdateFailed:
            description = QObject::tr("Failed to change account information.");
        break;
        case ErrorCodes::EncryptionFailure:
            description = QObject::tr("Encryption failed, please try again.");
        break;
        case ErrorCodes::PasswordTooShort:
            description = QObject::tr("The password is too short, please try again.");
        break;
        case ErrorCodes::Download:
            description = QObject::tr("Could not complete the download of the requested item.");
        break;
        case ErrorCodes::AccessDenied:
            description = QObject::tr("Could not complete the download of the requested item.");
        break;
        case ErrorCodes::RedeemCodeFailure:
            description = QObject::tr("The supplied points code was not valid.");
        break;

        case ErrorCodes::PurchaseMethodCreation:
            title = QObject::tr("Payment Method Error");
            description = QObject::tr("Failed to update purchase method details.");
        break;
        case ErrorCodes::PurchaseMethodDeletion:
            title = QObject::tr("Payment Method Error");
            description = QObject::tr("Failed to update purchase method details.");
        break;
        case ErrorCodes::PurchaseMethodDetails:
            title = QObject::tr("Payment Method Error");
            description = QObject::tr("Failed to update purchase method details.");
        break;
        case ErrorCodes::PurchaseMethodMissing:
            title = QObject::tr("Payment Method Error");
            description = QObject::tr("A valid payment method could not be found. Please add a payment method to your account.");
        break;
        case ErrorCodes::CustomerRetrieval:
            title = QObject::tr("Purchase Method Error");
            description = QObject::tr("No purchase method (e.g. credit card) information found.");
        break;
        case ErrorCodes::CustomerDatabase:
            description = QObject::tr("Could not locate the requested customer account.");
        break;
        case ErrorCodes::CustomerCharge:
        case ErrorCodes::CustomerRefund:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("Unable to complete the purchase process. No charge was made to your card.");
        break;
        case ErrorCodes::ChargeNotRecorded:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("Transaction was successful, but could not record the result on the account");
        break;
        case ErrorCodes::AccountNotEnoughPoints:
            title = QObject::tr("Purchase Error");
            description = QObject::tr("Your account lacks enough points to make this purchase.");
        break;
        case ErrorCodes::CollectionExists:
            title = QObject::tr("Creation Error");
            description = QObject::tr("The collection already exists.");
        break;
        case ErrorCodes::AssetExists:
            title = QObject::tr("Creation Error");
            description = QObject::tr("The asset already exists.");
        break;

        case ErrorCodes::CardDeclined:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("Your card has been declined. Please try another payment method");
        break;

        case ErrorCodes::CardIncorrectNumber:
        case ErrorCodes::CardInvalidNumber:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("The card number supplied is incorrect. Please check it and try again.");
        break;

        case ErrorCodes::CardInvalidExpiryMonth:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("The card expiry month supplied is incorrect.");
        break;

        case ErrorCodes::CardInvalidExpiryYear:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("The card expiry year supplied is incorrect.");
        break;

        case ErrorCodes::CardInvalidCVC:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("The card CVC supplied is incorrect.");
        break;

        case ErrorCodes::CardExpired:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("Your card has expired. Please try another payment method.");
        break;

        case ErrorCodes::CardInvalidAmount:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("The purchase amount was invalid. Please try again.");
        break;

        case ErrorCodes::CardDuplicateTransaction:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("This transaction was already processed.");
        break;

        case ErrorCodes::CardProcessingError:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("A processing error occurred when making the purchase. Please try again.");
        break;
        case ErrorCodes::PurchaseNotEnoughPoints:
            title = QObject::tr("Transaction Error");
            description = QObject::tr("Too few points were requested. A minimum of 500 points per purchase is required.");
        break;
        case ErrorCodes::PurchaseTooManyPoints:
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

Error::Error(ErrorCodes::ServerCode code)
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

ErrorCodes::ServerCode Error::serverCode() const
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

ErrorCodes::ServerCode Error::serverCodeId(const QString &string)
{
    Private::initServerCodeDict();
    if (Private::serverCodes.contains(string)) {
        return Private::serverCodes.value(string);
    }

    return ErrorCodes::Unknown;
}

#include "error.moc"

