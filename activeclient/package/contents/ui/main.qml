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


Image {
    id: appRoot
    source: "image://appbackgrounds/contextarea"
    fillMode: Image.Tile

    width: 360
    height: 360

    property string username
    property string password

    //Signon functions
    function authenticate(username, password)
    {
        bodegaClient.session.baseUrl = "http://127.0.0.1:3000"
        bodegaClient.session.deviceId = "VIVALDI-1"

        appRoot.username = username
        appRoot.password = password

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
        bodegaClient.saveCredentials(username, password)

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
        name: "com.coherenttheory.bodegastore"
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

}
