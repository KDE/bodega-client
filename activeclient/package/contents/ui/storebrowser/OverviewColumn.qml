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
import org.kde.plasma.mobilecomponents 0.1 as MobileComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import "../components"

BrowserColumn {
    id: root
    clip: true

    ListView {
        id: categoriesView
        currentIndex: -1
        clip: true
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: statusFrame.top
        }
        model: VisualDataModel {
            model: bodegaClient.channelsModel
            delegate: StoreListItem {
                checked: categoriesView.currentIndex == index
                onClicked: {
                    if (categoriesView.currentIndex == index) {
                        return
                    }
                    categoriesView.currentIndex = index
                    itemBrowser.pop(root)
                    var channels = itemBrowser.push(Qt.createComponent("ChannelsColumn.qml"))
                    channels.rootIndex = categoriesView.model.modelIndex(index)
                    channels.channelId = model.ChannelIdRole
                }
            }
        }
    }
    PlasmaComponents.ScrollBar {
        flickableItem: categoriesView
        orientation: Qt.Vertical
    }
    PlasmaCore.FrameSvgItem {
        id: statusFrame
        imagePath: "widgets/frame"
        prefix: "raised"
        enabledBorders: "TopBorder"
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            topMargin: 8
        }
        height: childrenRect.height + margins.top
        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            PlasmaComponents.Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: bodegaClient.session.userName
            }
            PlasmaComponents.Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18ncp("number of points left", "1 point", "%1 points", bodegaClient.session.points)
            }
            PlasmaComponents.Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18n("Account...")
                onClicked: mainStack.push(Qt.createComponent("../SettingsPage.qml"))
            }
        }
    }
}
