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

    Column {
        spacing: 8
        anchors.centerIn: parent
        PlasmaComponents.Label {
            id: heading
            anchors.horizontalCenter: parent.horizontalCenter
            text: i18n("Congratulations! Your new account has been registered.")
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            width: root.width * 2 / 3
        }

        PlasmaComponents.Label {
            id: info
            anchors.horizontalCenter: parent.horizontalCenter
            text: i18n("Please check your email for a confirmation link,\nthen press the \"Connect!\" button below.")
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            width: heading.width
        }

        PlasmaComponents.Button {
            id: submitButton
            text: i18n("Connect!")
            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                print("Going to connect now with " + bodegaClient.session.userName + " and " + bodegaClient.session.password);
                mainStack.push(Qt.createComponent("ConnectingPage.qml"));
                appRoot.authenticate(bodegaClient.session.userName, bodegaClient.session.password);
            }
        }
    }
}
