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

#include <QDebug>
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

    ServerCode code;
    Type type;
    QString id;
    QString title;
    QString description;
};

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
        case Unknown:
            description = QObject::tr("An unknown error occurred.");
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

        NoCode:
        break;
    }
}

Error::Error()
    : d(new Private(Session))
{
    qDebug() << "********************** 1" << d->code << d->type << d->id << d->title << d->description;
}

Error::Error(ServerCode code)
    : d(new Private(code))
{
    qDebug() << "********************** 2" << d->code << d->type << d->id << d->title << d->description;
}

Error::Error(Type type, const QString &msg)
    : d(new Private(type, msg, msg, msg))
{
    qDebug() << "********************** 3" << d->code << d->type << d->id << d->title << d->description;
}

Error::Error(Type type,
             const QString &errorId,
             const QString &title,
             const QString &descr)
    : d(new Private(type, errorId, title, descr))
{
    qDebug() << "********************** 4" << d->code << d->type << d->id << d->title << d->description;
}

Error::~Error()
{
    delete d;
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

