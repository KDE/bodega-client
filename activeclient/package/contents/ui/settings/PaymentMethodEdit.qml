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
            }
            PlasmaComponents.TextField {
                id: numberField2
            }
            PlasmaComponents.TextField {
                id: numberField3
            }
            PlasmaComponents.TextField {
                id: numberField4
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
        }

        PlasmaComponents.Label {
            text: i18n("Billing address (line2):")
            anchors {
                right: address2Field.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        PlasmaComponents.TextField {
            id: address2Field
            width: cwField.width * 2
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
        }

        Item{}
        PlasmaComponents.Button {
            text: i18n("Save")
        }
    }

    function loadData()
    {
        busyIndicator.visible = true;
        busyIndicator.running = true;
        job = bodegaClient.session.paymentMethod();
        job.jobFinished.connect(jobFinished);
    }

    function jobFinished()
    {
        if (job.failed) {
            showMessage(job.error.title, job.error.id + ": " + job.error.description);
        }

        for (var i in job.parsedJson) {
            print(i+": "+job.parsedJson[i])
        }
        var cardData = job.parsedJson

        for (var i = 0; i < cardColumn.children.length; ++i) {
            if (cardColumn.children[i].text == cardData.type) {
                cardColumn.children[i].checked = true;
                break;
            }
        }

        countryField.text = cardData.address_country;
        address1Field.text = cardData.address_line1;
        address2Field.text = cardData.address_line2;
        stateField.text = cardData.address_state;
        zipField.text = cardData.address_zip;
        
        
        busyIndicator.running = false;
        busyIndicator.visible = false;
    }

    Component.onCompleted: loadData()
}
