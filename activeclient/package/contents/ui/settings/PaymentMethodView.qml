/*
 *   Copyright 2012 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.qtextracomponents 0.1
import "../components"


PlasmaComponents.Page {
    id: root

    property variant job

    Column {
        id: mainColumn
        anchors.centerIn: parent

        Grid {
            id: mainGrid
            spacing: 4
            columns: 2
            rows: 13

            PlasmaComponents.Label {
                text: i18n("Card type:")
                anchors {
                    right: cardTypeLabel.left
                    rightMargin: theme.defaultFont.mSize.width
                }
            }
            PlasmaComponents.Label {
                id: cardTypeLabel
                width: Math.max(paintedWidth, 1)
                text: paymentMethodStack.cardType
            }

            PlasmaComponents.Label {
                text: i18n("Name:")
                anchors {
                    right: nameLabel.left
                    rightMargin: theme.defaultFont.mSize.width
                }
            }
            PlasmaComponents.Label {
                id: nameLabel
                text: paymentMethodStack.name
            }

            PlasmaComponents.Label {
                text: i18n("Number:")
                anchors {
                    right: numberLabel.left
                    rightMargin: theme.defaultFont.mSize.width
                }
            }
            PlasmaComponents.Label {
                id: numberLabel
                text: "**** **** **** " + paymentMethodStack.last4
            }

            PlasmaComponents.Label {
                text: i18n("Billing address:")
                visible: address1Label.visible
                anchors {
                    right: address1Label.left
                    rightMargin: theme.defaultFont.mSize.width
                }
            }
            PlasmaComponents.Label {
                id: address1Label
                visible: text.length > 0
                width: Math.max(paintedWidth, 1)
                text: paymentMethodStack.address1
            }

            //Spacer
            Item {width: 1; height: 1}
            PlasmaComponents.Label {
                id: address2Label
                visible: text.length > 0
                width: Math.max(paintedWidth, 1)
                text: paymentMethodStack.address2
            }

            PlasmaComponents.Label {
                text: i18n("Country:")
                visible: countryLabel.visible
                anchors {
                    right: countryLabel.left
                    rightMargin: theme.defaultFont.mSize.width
                }
            }
            PlasmaComponents.Label {
                id: countryLabel
                visible: text.length > 0
                width: Math.max(paintedWidth, 1)
                text: paymentMethodStack.country
            }

            PlasmaComponents.Label {
                text: i18n("State:")
                visible: stateLabel.visible
                anchors {
                    right: stateLabel.left
                    rightMargin: theme.defaultFont.mSize.width
                }
            }
            PlasmaComponents.Label {
                id: stateLabel
                visible: text.length > 0
                width: Math.max(paintedWidth, 1)
                text: paymentMethodStack.state
            }

            PlasmaComponents.Label {
                text: i18n("ZIP:")
                visible: zipLabel.visible
                anchors {
                    right: zipLabel.left
                    rightMargin: theme.defaultFont.mSize.width
                }
            }
            PlasmaComponents.Label {
                id: zipLabel
                visible: text.length > 0
                width: Math.max(paintedWidth, 1)
                text: paymentMethodStack.zip
            }
        }
        Column {
            anchors {
                top: mainGrid.bottom
                horizontalCenter: mainGrid.horizontalCenter
            }
            spacing: 4
            PlasmaComponents.Button {
                text: i18n("Update")
                onClicked: paymentMethodStack.replace(Qt.createComponent("PaymentMethodEdit.qml"))
            }
            PlasmaComponents.Button {
                text: i18n("Delete")
                onClicked: {
                    paymentMethodStack.deletePaymentMethod();
                }
            }
        }
    }
}
