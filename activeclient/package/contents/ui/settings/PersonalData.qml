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

    property bool creation: false
    property int spacing: 4
    property variant job

    //FIXME: proper solution needed
    function showMessage(title, message)
    {
        print(title + ": " + message);
    }

    function hideMessage()
    {
    }

    Grid {
        anchors.centerIn: parent
        rows: 6
        columns: 2
        spacing: root.spacing

        PlasmaComponents.Label {
            text: i18n("Name:")
            anchors {
                right: nameField.left
                rightMargin: root.spacing
            }
        }
        PlasmaComponents.TextField {
            id: nameField
        }

        PlasmaComponents.Label {
            text: i18n("Last name:")
            anchors {
                right: lastNameField.left
                rightMargin: root.spacing
            }
        }
        PlasmaComponents.TextField {
            id: lastNameField
        }

        PlasmaComponents.Label {
            text: i18n("Email:")
            anchors {
                right: emailField.left
                rightMargin: root.spacing
            }
        }

        PlasmaComponents.TextField {
            id: emailField
        }

        PlasmaComponents.Label {
            text: i18n("Password:")
            anchors {
                right: passwordField.left
                rightMargin: root.spacing
            }
        }
        PlasmaComponents.TextField {
            id: passwordField
            echoMode: TextInput.Password
        }

        PlasmaComponents.Label {
            text: i18n("Password (repeat):")
            anchors {
                right: password2Field.left
                rightMargin: root.spacing
            }
        }

        PlasmaComponents.TextField {
            id: password2Field
            echoMode: TextInput.Password
            Row {
                opacity: passwordField.text ? 1 : 0
                anchors.left: parent.right
                QIconItem {
                    width: theme.smallMediumIconSize
                    height: width
                    icon: (passwordField.text == password2Field.text) ? "dialog-ok" : "dialog-cancel"
                }
                PlasmaComponents.Label {
                    opacity: passwordField.text != password2Field.text ? 1 : 0
                    text: i18n("Password mismatch")
                    Behavior on opacity {
                        NumberAnimation {
                            duration: 250
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
                Behavior on opacity {
                    NumberAnimation {
                        duration: 250
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }

        //just a placeholder
        Item {
            width: 10
            height: 10
        }
        PlasmaComponents.Button {
            id: saveButton
            text: creation ? i18n("Create") : i18n("Loading...")
            enabled: nameField.text && lastNameField.text && emailField.text && (passwordField.text == password2Field.text)
            property variant pwordJob
            property string newPword

            onClicked: {
                if (creation) {
                    mainStack.push(Qt.createComponent("../ConnectingPage.qml"))
                } else {
                    if (passwordField.text != '') {
                        newPword = passwordField.text;
                        pwordJob = bodegaClient.session.changePassword(passwordField.text);
                        pwordJob.jobFinished.connect(pwordResetDone);
                    }
                }

                enabled = false;
            }

            function pwordResetDone()
            {
                if (pwordJob.failed) {
                    showMessage(pwordJob.error.title, pwordJob.error.errorId + ': ' + pwordJob.error.description);
                } else {
                    bodegaClient.session.password = newPword;
                    authenticate(bodegaClient.session.userName, newPword)
                }

                passwordField.text = '';
                password2Field.text = '';
                enabled = nameField.text && lastNameField.text && emailField.text && (passwordField.text == password2Field.text);
            }

            PlasmaComponents.BusyIndicator {
                id: busyIndicator
                visible: false
                height: saveButton.height
                width: height
                anchors {
                    left: parent.right
                }
            }
        }
    }

    function loadData()
    {
        busyIndicator.visible = true;
        busyIndicator.running = true;
        job = bodegaClient.session.participantInfo("fuck");
        job.jobFinished.connect(jobFinished);
        job.infoReceived.connect(displayInfo);
    }

    function jobFinished()
    {
        if (job.failed) {
            showMessage(job.error.title, job.error.id + ": " + job.error.description);
        }

        busyIndicator.running = false;
        busyIndicator.visible = false;
        saveButton.text = i18n("Save")
    }

    function displayInfo(info)
    {
        nameField.text = info.firstName;
        lastNameField.text = info.lastName;
        emailField.text = info.email;
    }

    Component.onCompleted: {
        if (!creation) {
            loadData();
        }
    }
}
