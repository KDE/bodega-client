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
import "./components"


SimplePage {
    id: root
    objectName: "passwordPage"
    title: "Make Play Live"


    Column {
        anchors.centerIn: parent
        spacing: 8
        Grid {
            spacing: 8
            rows: 2
            columns: 2
            PlasmaComponents.Label {
                text: i18n("Email:")
                anchors {
                    right: emailField.left
                    rightMargin: 8
                }
            }
            PlasmaComponents.TextField {
                id: emailField
                Keys.onTabPressed: passwordField.forceActiveFocus()
                Keys.onPressed: {
                    if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return) {
                        submitButton.submit()
                    }
                }
            }
            PlasmaComponents.Label {
                text: i18n("Password:")
                anchors {
                    right: passwordField.left
                    rightMargin: 8
                }
            }
            PlasmaComponents.TextField {
                id: passwordField
                echoMode: TextInput.Password
                Keys.onTabPressed: emailField.forceActiveFocus()
                Keys.onPressed: {
                    if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return) {
                        submitButton.submit()
                    }
                }
            }
        }
        PlasmaComponents.Button {
            id: submitButton
            text: i18n("Connect!")
            anchors.horizontalCenter: parent.horizontalCenter
            enabled: emailField.text && passwordField.text
            onClicked: submit()

            function submit()
            {
                if (!submitButton.enabled) {
                    return
                }

                appRoot.authenticate(emailField.text, passwordField.text)
            }
        }
    }

    PlasmaComponents.ToolButton {
        text: i18n("Forgot password?")
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 8
        }
    }
}
