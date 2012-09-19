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

    Grid {
        id: grid
        anchors.centerIn: parent

        rows: 8
        columns: 2
        spacing: root.spacing

        PlasmaComponents.Label {
            text: i18n("Name:")
            id: nameLabel
            anchors {
                right: lastNameField.left
                rightMargin: root.spacing
            }
        }
        PlasmaComponents.TextField {
            id: nameField
            width: theme.defaultFont.mSize.width * 20
            Keys.onTabPressed: lastNameField.forceActiveFocus()
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
            width: nameField.width
            Keys.onTabPressed: emailField.forceActiveFocus()
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
            width: nameField.width
            Keys.onTabPressed: creation ? passwordField.forceActiveFocus() : saveInfoButton.forceActiveFocus()
        }

        //just a placeholder
        Item {
            visible: !creation
            width: 1
            height: saveInfoButton.height
        }
        PlasmaComponents.Button {
            id: saveInfoButton
            visible: !creation
            text: i18n("Loading...")
            enabled: nameField.text && lastNameField.text && emailField.text
            property variant job

            onClicked: {
                text = i18n("Saving...")
                enabled = false;

                job = bodegaClient.session.changeAccountDetails(nameField.text, lastNameField.text, emailField.text);
                job.jobFinished.connect(updateDone);
                job.jobError.connect(updateFailed)
            }

            function updateDone()
            {
                if (job.failed) {
                    showMessage(job.error.title, job.error.errorId + ': ' + job.error.description);
                }

                enabled = nameField.text && lastNameField.text && emailField.text
                text = i18n("Update Account")
            }

            function updateFailed(job, error)
            {
                showMessage(error.title, error.description, saveInfoButton)
            }

            PlasmaComponents.BusyIndicator {
                id: infoSaveBusyIndicator
                visible: false
                height: savePasswordButton.height
                width: height
                anchors {
                    left: parent.right
                }
            }

            Keys.onTabPressed: passwordField.forceActiveFocus()
        }

        Item {
            width: 1
            height: creation ? 0 : root.spacing * 2
        }
        Item {
            width: 1
            height: creation ? 0 : root.spacing * 2
        }

        PlasmaComponents.Label {
            text: i18n("New password:")
            anchors {
                right: passwordField.left
                rightMargin: root.spacing
            }
        }
        PlasmaComponents.TextField {
            id: passwordField
            echoMode: TextInput.Password
            width: nameField.width
            Keys.onTabPressed: password2Field.forceActiveFocus()
        }

        PlasmaComponents.Label {
            text: i18n("Confirm password:")
            anchors {
                right: password2Field.left
                rightMargin: root.spacing
            }
        }

        PlasmaComponents.TextField {
            id: password2Field
            echoMode: TextInput.Password
            width: nameField.width
            Keys.onTabPressed: savePasswordButton.forceActiveFocus()
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
            width: 1
            height: savePasswordButton.height
        }
        PlasmaComponents.Button {
            id: savePasswordButton
            text: creation ? i18n("Create") : i18n("Update Password")
            enabled: passwordField.text != '' && passwordField.text == password2Field.text
            width: emailField.width
            property variant pwordJob
            property variant registerJob
            property string newPword

            onClicked: {
                if (creation) {
                    registerJob = bodegaClient.session.registerAccount(
                                      emailField.text,
                                      passwordField.text,
                                      nameField.text,
                                      "",
                                      lastNameField.text);
                    registerJob.jobFinished.connect(registerAccountDone);
                    registerJob.jobError.connect(passwordUpdateFailed);
                } else {
                    if (passwordField.text != '') {
                        newPword = passwordField.text;
                        pwordJob = bodegaClient.session.changePassword(passwordField.text);
                        pwordJob.jobFinished.connect(pwordResetDone);
                        pwordJob.jobError.connect(passwordUpdateFailed);
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

                mainStack.pop()
                passwordField.text = '';
                password2Field.text = '';
                enabled = passwordField.text == password2Field.text;
            }

            function registerAccountDone()
            {
                if (!registerJob.failed) {
                    bodegaClient.session.userName = emailField.text;
                    bodegaClient.session.password = passwordField.text;
                    bodegaClient.saveCredentials();
                    mainStack.push(Qt.createComponent("../AccountCreatedPage.qml"));
                }
            }

            function passwordUpdateFailed(job, error)
            {
                showMessage(error.title, error.description, savePasswordButton)
                savePasswordButton.enabled = true
            }

            PlasmaComponents.BusyIndicator {
                id: passwordSaveBusyIndicator
                visible: false
                height: savePasswordButton.height
                width: height
                anchors {
                    left: parent.right
                }
            }
        }
    }

    function loadData()
    {
        var indicator = creation ? passwordSaveBusyIndicator : infoSaveBusyIndicator
        indicator.visible = true;
        indicator.running = true;
        job = bodegaClient.session.participantInfo();
        job.jobFinished.connect(jobFinished);
        job.infoReceived.connect(displayInfo);
    }

    function jobFinished()
    {
        if (job.failed) {
            showMessage(job.error.title, job.error.id + ": " + job.error.description);
        }

        infoSaveBusyIndicator.running = false;
        infoSaveBusyIndicator.visible = false;
        saveInfoButton.text = i18n("Update Account")
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
