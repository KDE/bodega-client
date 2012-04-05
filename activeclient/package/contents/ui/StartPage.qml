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
    objectName: "startPage"
    title: "Make Play Live"

    Column {
        anchors.centerIn: parent
        spacing: 12
        PlasmaComponents.ToolButton {
            anchors.horizontalCenter: parent.horizontalCenter;
            text: i18n("Add an existing account")
            iconSource: "list-add"
            font.pointSize: theme.defaultFont.pointSize * 1.5
            onClicked: mainStack.push(Qt.createComponent("PasswordPage.qml"))
        }
        PlasmaComponents.ToolButton {
            anchors.horizontalCenter: parent.horizontalCenter;
            text: i18n("Create a new account")
            iconSource: "contact-new"
            font.pointSize: theme.defaultFont.pointSize * 1.5
            onClicked: mainStack.push(Qt.createComponent("CreateUser.qml"))
        }
    }
}
