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
        rows: 11

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
            text: i18n("Name on card:")
            anchors {
                right: holderField.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        PlasmaComponents.TextField {
            id: holderField
            width: cvcField.width * 2
            text: paymentMethodStack.name
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
                inputMask: "9999"
                width: Math.min(implicitWidth, root.width/5)-10
                onTextChanged: {
                    if (cursorPosition >= 4) {
                        numberField2.forceActiveFocus()
                        numberField2.cursorPosition = 0
                    }
                }
                Keys.onPressed: {
                    if (text.length == 0) {
                        numberField1.cursorPosition = 0
                    }
                }
            }
            PlasmaComponents.TextField {
                id: numberField2
                inputMask: "9999"
                width: numberField1.width
                onTextChanged: {
                    if (cursorPosition >= 4) {
                        numberField3.forceActiveFocus()
                        numberField3.cursorPosition = 0
                    }
                }
            }
            PlasmaComponents.TextField {
                id: numberField3
                inputMask: "9999"
                width: numberField1.width
                onTextChanged: {
                    if (cursorPosition >= 4) {
                        numberField4.forceActiveFocus()
                        numberField4.cursorPosition = 0
                    }
                }
            }
            PlasmaComponents.TextField {
                id: numberField4
                inputMask: "9999"
                width: numberField1.width
                text: paymentMethodStack.last4
            }
        }

        PlasmaComponents.Label {
            text: i18n("cvc:")
            anchors {
                right: cvcRow.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        Row {
            id: cvcRow
            PlasmaComponents.TextField {
                id: cvcField
                width: numberField1.width
            }
            PlasmaComponents.ToolButton {
                id: cvcHelpButton
                text: i18n("?")
                width: height
                onClicked: showMessage(i18n("Help"), i18n("The CVC (Card Verification Code) is the 3-digit number located on the back of your card, usually at the top of the signature strip. By including it with your order, you help protect your account from fraud and you give us one more way of being sure your order is legitimate."), cvcHelpButton)
            }
        }

        PlasmaComponents.Label {
            text: i18n("Expires:")
            anchors {
                right: expiryRow.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        Row {
            id: expiryRow
            PlasmaComponents.TextField {
                id: expiryMonth
                width: cvcField.width
                inputMask: "99"
                validator: IntValidator {
                    bottom: 1
                    top: 12
                }
                onTextChanged: {
                    if (cursorPosition >= 2 && acceptableInput) {
                        expiryYear.forceActiveFocus()
                        expiryYear.cursorPosition = 0
                    }
                }
                Keys.onPressed: {
                    if (text.length == 0) {
                        expiryMonth.cursorPosition = 0
                    }
                }
            }

            PlasmaComponents.Label {
                text: "/";
            }

            PlasmaComponents.TextField {
                id: expiryYear
                width: cvcField.width
                inputMask: "9999"
            }
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
            width: cvcField.width * 2
            text: paymentMethodStack.address1
        }

        Item {width: 1; height: 1}
        PlasmaComponents.TextField {
            id: address2Field
            width: cvcField.width * 2
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
            id: saveButton
            text: i18n("Save")
            onClicked: {
                if (expiryYear.text.length == 2) {
                    expiryYear.text = "20" + expiryYear.text;
                }

                job = bodegaClient.session.setPaymentMethod(numberField1.text + numberField2.text + numberField3.text + numberField4.text,
                                        expiryMonth.text, expiryYear.text,
                                        cvcField.text, holderField.text,
                                        address1Field.text,
                                        address2Field.text,
                                        zipField.text,
                                        stateField.text,
                                        countryField.text)

                job.jobFinished.connect(jobFinished);
            }
        }
    }

    function jobFinished()
    {
        if (job.failed) {
            showMessage(job.error.title, job.error.errorId + ": " + job.error.description);

            print("Error")
            for (var i in job.error) {
                print(i + ": " + job.error[i])
            }
            showMessage(i18n("Error"), buyJob.error.description, saveButton)
        } else {
            print("Answer")
            var response = job.parsedJson
            for (var i in response) {
                print(i + ": " + response[i])
            }
            root.pageStack.clear()
            root.pageStack.loadData()
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
