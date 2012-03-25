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
import org.kde.plasma.extras 0.1 as PlasmaExtraComponents
import org.kde.qtextracomponents 0.1
import "../components"


PlasmaComponents.Page {
    id: root

    Column {
        id: mainColumn
        spacing: 4
        anchors.centerIn: parent

        PlasmaExtraComponents.Heading {
            text: i18n("Redeem Points Code")
            level: 4
        }

        Grid {
            rows: 2
            columns: 2
            spacing: 4

            PlasmaComponents.Label {
                text: i18n("Code:")
                anchors {
                    right: code.left
                    rightMargin: mainColumn.spacing
                }
            }

            PlasmaComponents.TextField {
                id: code
                onTextChanged: { redemptionButton.enabled = text.length > 0; }
            }

            PlasmaComponents.Button {
                id: redemptionButton
                text: i18n("Redeem code");
                enabled: false

                anchors {
                    left: code.left
                }

                onClicked: {
                    redeemBusy.running = true;
                    redeemBusy.visible = true;
                    enabled = false;
                }
            }

            PlasmaComponents.BusyIndicator {
                id: redeemBusy;
                visible: false;
                height: redemptionButton.height
                width: height
                anchors {
                    left: redemptionButton.right
                }
            }
        }

        PlasmaExtraComponents.Heading {
            text: i18n("Purchase Points")
            level: 4
        }

        PlasmaComponents.ButtonColumn {
            id: buttonColumn
            exclusive: true
            spacing: mainColumn.spacing

            PlasmaComponents.RadioButton {
                text: i18n("500 points")
            }
            PlasmaComponents.RadioButton {
                text: i18n("1000 points")
            }
            PlasmaComponents.RadioButton {
                text: i18n("5000 points")
            }
            PlasmaComponents.RadioButton {
                text: i18n("10,000 points")
            }
            PlasmaComponents.RadioButton {
                text: i18n("Other:")
                onCheckedChanged: {
                    if (checked) {
                        otherField.forceActiveFocus()
                    }
                }
                PlasmaComponents.TextField {
                    id: otherField
                    anchors {
                        left: parent.right
                        leftMargin: mainColumn.spacing * 2
                    }
                }
            }
        }

        PlasmaComponents.Button {
            text: i18n("Purchase")
        }
    }
}
