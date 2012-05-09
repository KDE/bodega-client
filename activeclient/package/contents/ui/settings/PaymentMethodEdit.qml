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

    //TODO: meaningful position
    PlasmaComponents.BusyIndicator {
        id: busyIndicator
        visible: false
        anchors.centerIn: parent
    }
    Grid {
        id: mainColumn
        spacing: 4
        anchors.centerIn: parent
        columns: 2
        rows: 10

        PlasmaComponents.Label {
            text: i18n("Card type:")
            anchors {
                right: cardColumn.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        PlasmaComponents.ButtonColumn {
            id: cardColumn
            exclusive: true
            PlasmaComponents.RadioButton {
                text: "Visa"
            }
            PlasmaComponents.RadioButton {
                text: "Master Card"
            }
            PlasmaComponents.RadioButton {
                text: "Diners Club"
            }
        }
        
        
        PlasmaComponents.Label {
            text: i18n("Number:")
            anchors {
                right: numberRow.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        Row {
            id: numberRow
            PlasmaComponents.TextField {
                id: numberField1
                onTextChanged: {
                    if (text.length >= 4) {
                        numberField2.forceActiveFocus()
                    }
                }
            }
            PlasmaComponents.TextField {
                id: numberField2
                onTextChanged: {
                    if (text.length >= 4) {
                        numberField3.forceActiveFocus()
                    }
                }
            }
            PlasmaComponents.TextField {
                id: numberField3
                onTextChanged: {
                    if (text.length >= 4) {
                        numberField4.forceActiveFocus()
                    }
                }
            }
            PlasmaComponents.TextField {
                id: numberField4
                text: paymentMethodStack.last4
            }
        }

        PlasmaComponents.Label {
            text: i18n("CW:")
            anchors {
                right: cwRow.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        Row {
            id: cwRow
            PlasmaComponents.TextField {
                id: cwField
            }
            PlasmaComponents.ToolButton {
                id: cwHelpButton
                text: i18n("?")
                width: height
                onClicked: showMessage(i18n("Help"), i18n("explanation what is a CW"), cwHelpButton)
            }
        }

        PlasmaComponents.Label {
            text: i18n("Expiry month/Year:")
            anchors {
                right: expiryField.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        PlasmaComponents.TextField {
            id: expiryField
            inputMask: "99/9999"
        }
        
        PlasmaComponents.Label {
            text: i18n("Billing address:")
            anchors {
                right: address1Field.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        PlasmaComponents.TextField {
            id: address1Field
            width: cwField.width * 2
            text: paymentMethodStack.address1
        }

        Item {width: 1; height: 1}
        PlasmaComponents.TextField {
            id: address2Field
            width: cwField.width * 2
            text: paymentMethodStack.address2
        }

        PlasmaComponents.Label {
            text: i18n("Country:")
            anchors {
                right: countryField.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        PlasmaComponents.TextField {
            id: countryField
            text: paymentMethodStack.country
        }

        PlasmaComponents.Label {
            text: i18n("State:")
            anchors {
                right: stateField.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        PlasmaComponents.TextField {
            id: stateField
            text: paymentMethodStack.state
        }

        PlasmaComponents.Label {
            text: i18n("ZIP:")
            anchors {
                right: zipField.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        PlasmaComponents.TextField {
            id: zipField
            text: paymentMethodStack.zip
        }

        Item {width: 1; height: 1}
        PlasmaComponents.Button {
            text: i18n("Save")
        }
    }

    Connections {
        target: paymentMethodStack
        onCardTypeChanged: {
            for (var i = 0; i < cardColumn.children.length; ++i) {
                if (cardColumn.children[i].text == paymentMethodStack.cardType) {
                    cardColumn.children[i].checked = true;
                    break;
                }
            }
        }
    }
}
