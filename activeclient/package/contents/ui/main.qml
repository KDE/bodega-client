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
import org.kde.plasma.mobilecomponents 0.1 as MobileComponents
import org.kde.qtextracomponents 0.1
import "storebrowser"
import "components"


Image {
    id: appRoot
    source: "image://appbackgrounds/contextarea"
    fillMode: Image.Tile

    width: 360
    height: 360

    function showMessage(title, message, visualParent)
    {
        if (visualParent) {
            inlineMessage.title = title
            inlineMessage.message = message
            inlineMessage.visualParent = visualParent
            inlineMessage.open()
        } else {
            messageBox.title = title
            messageBox.text = message
            messageBox.state = "right"
            messageBox.state = ""
        }
    }
    function hideMessage()
    {
        messageBox.state = "left"
    }

    InlineMessage {
        id: inlineMessage
    }

    //Signon functions
    function authenticate(username, password)
    {
        if (username == '' || password == '') {
            if (!connectPageTimer.authing) {
                connectPageTimer.restart()
            }
        } else {
            bodegaClient.session.userName = username
            bodegaClient.session.password = password

            var job = bodegaClient.session.signOn()
            job.signedOn.connect(signedOn)
            job.jobError.connect(errorSigning)
        }
    }

    function signedOn(job)
    {
        //save credentials only if login worked
        bodegaClient.saveCredentials()

        connectPageTimer.running = false
        mainStack.replace(Qt.createComponent("storebrowser/ItemBrowser.qml"), '', true)
    }

    function errorSigning(job, error)
    {
        connectPageTimer.running = false
        while (mainStack.currentPage.objectName != "startPage" &&
               mainStack.currentPage.objectName != "passwordPage") {
            mainStack.pop(0, true)
        }

        if (!mainStack.currentPage.objectName != "passwordPage") {
            mainStack.push(Qt.createComponent("PasswordPage.qml"))
        }

        mainStack.currentPage.email = bodegaClient.session.userName
        mainStack.currentPage.showMessage(error.title, error.description)
    }


    //Tis timer is to avoid to show the connecting page if the authentication is fast enough
    Timer {
        id: connectPageTimer
        interval: 100
        property bool authing
        onTriggered: {
            authing = true
            var credentials = bodegaClient.retrieveCredentials()
            authenticate(credentials.username, credentials.password)
            authing = false
        }
    }

    //Give it time to show a window before doing any expensive job
    Timer {
        interval: 100
        running: true
        onTriggered: {
            var credentials = bodegaClient.retrieveCredentials()
            if (credentials.username && credentials.password) {
                mainStack.push(Qt.createComponent("ConnectingPage.qml"), "", true)
                connectPageTimer.start()
            }
        }
    }

    MobileComponents.Package {
        id: viewerPackage
        name: "com.coherenttheory.addonsapp"
    }

    PlasmaComponents.BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: false
        running: visible
    }


    PlasmaComponents.PageStack {
        id: mainStack
        clip: false
        initialPage: Qt.createComponent("StartPage.qml")
        anchors {
            fill: parent
        }
    }

    PlasmaCore.FrameSvgItem {
        id: messageBox
        z: 9999
        property alias title: messageTitleLabel.text
        property alias text: messageLabel.text
        state: "right"

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: theme.defaultFont.mSize.height * 2
        }

        imagePath: "dialogs/background"
        prefix: "raised"
        width: parent.width/3
        height: childrenRect.height + margins.top + margins.bottom
        Column {
            id: mainColumn
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                leftMargin: parent.margins.left
                rightMargin: parent.margins.right
                topMargin: parent.margins.top
            }
            PlasmaComponents.Label {
                id: messageTitleLabel
                anchors.horizontalCenter: parent.horizontalCenter
            }
            PlasmaComponents.Label {
                id: messageLabel

                wrapMode: Text.WordWrap
                horizontalAlignment: paintedHeight > theme.defaultFont.mSize.height ? Text.AlignJustify : Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                }
            }
        }
        MouseArea {
            anchors.fill: mainColumn
            onClicked: appRoot.hideMessage()
        }

        transform: Translate {
            id: translateTransform
        }
        states: [
            State {
                name: ""
                PropertyChanges {
                    target: translateTransform
                    x: 0
                }

            },
            State {
                name: "right"
                PropertyChanges {
                    target: translateTransform
                    x: appRoot.width - messageBox.x
                }
            },
            State {
                name: "left"
                PropertyChanges {
                    target: translateTransform
                    x: - messageBox.x - messageBox.width
                }
            }
        ]
        transitions: [
            Transition {
                from: "right"
                to: ""
                NumberAnimation {
                    targets: translateTransform
                    properties: "x"
                    duration: 250
                    easing.type: "OutQuad"
                }
            },
            Transition {
                from: ""
                to: "left"
                NumberAnimation {
                    targets: translateTransform
                    properties: "x"
                    duration: 250
                    easing.type: "InQuad"
                }
            }
        ]
    }
}
