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
    property alias email: emailField.text

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
                width: theme.defaultFont.mSize.width * 20
                Keys.onTabPressed: passwordField.forceActiveFocus()
                Keys.onPressed: {
                    if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return) {
                        submitButton.submit()
                    }
                }
            }
            PlasmaComponents.Label {
                id: passwordLabel
                text: i18n("Password:")
                anchors {
                    right: passwordField.left
                    rightMargin: 8
                }
            }
            PlasmaComponents.TextField {
                id: passwordField
                visible: passwordLabel.visible
                echoMode: TextInput.Password
                width: emailField.width
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
            anchors.right: parent.right
            enabled: emailField.text && (!passwordField.visible || passwordField.text)
            onClicked: submit()
            width: emailField.width
            property variant job

            function submit()
            {
                if (!submitButton.enabled) {
                    return;
                }

                if (passwordField.visible) {
                    appRoot.authenticate(emailField.text, passwordField.text);
                } else {
                    job = bodegaClient.session.resetPassword(emailField.text);
                    job.jobFinished.connect(showResetInstructions);
                }
            }

            function showResetInstructions()
            {
                 if (job.failed) {
                     showMessage(job.error.title, job.error.id + ": " + job.error.description);
                 } else {
                     showMessage(i18n("Password Reset Successfully"), i18n("Please check your email for further instructions."));
                 }
            }
        }
    }

    PlasmaComponents.ToolButton {
        text: i18n("Forgot your password?")
        visible: passwordField.visible

        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 8
        }

        onClicked: {
            visible = false;
            passwordLabel.visible = false;
            submitButton.text = i18n("Reset my password")
        }
    }

    PlasmaComponents.ToolButton {
        id: backButton
        iconSource: "go-previous"
        width: theme.largeIconSize
        height: width
        flat: false
        onClicked: mainStack.pop()
        anchors {
            bottom: parent.bottom
            left: parent.left
            margins: theme.defaultFont.mSize.height
        }
    }
}
