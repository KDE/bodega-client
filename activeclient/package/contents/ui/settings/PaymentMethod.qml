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

    Grid {
        id: mainColumn
        spacing: 4
        anchors.centerIn: parent
        columns: 2
        rows: 5

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
                text: i18n("Visa")
            }
            PlasmaComponents.RadioButton {
                text: i18n("Master Card")
            }
            PlasmaComponents.RadioButton {
                text: i18n("Diners Club")
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
        }
        
        PlasmaComponents.Label {
            text: i18n("Billing address:")
            anchors {
                right: addressField.left
                rightMargin: theme.defaultFont.mSize.width
            }
        }
        PlasmaComponents.TextField {
            id: addressField
        }
    }
}
