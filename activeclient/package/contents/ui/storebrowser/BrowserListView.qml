/*
 *   Copyright 2013 Giorgos Tsiapaliokas <terietor@gmail.com>
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
import org.kde.plasma.extras 0.1 as PlasmaExtras
import "../components"

BrowserColumn {
    id: root
    clip: true
    property alias abstractItemModel: visualDataModel.model
    property alias customDelegate: listView.delegate
    property alias view: listView
    property alias customHeader: listView.header
    property alias browserColumn: root

    property variant rootIndex

    VisualDataModel {
        id: visualDataModel
        rootIndex: root.rootIndex
        delegate: listView.delegate
    }

    PlasmaExtras.ScrollArea {
        anchors.fill:parent
        ListView {
            id: listView
            currentIndex: -1
            clip: true
            anchors.fill:parent
            model: visualDataModel

            footer: PlasmaComponents.ListItem {
                id: loaderFooter
                visible: false
                Connections {
                    target: listView
                    onAtYEndChanged: {
                        console.log(typeof visualDataModemodel)
                        console.log(typeof bodegaClient.session.installJobsModel)
                        console.log(typeof visualDataModel.model)
                        console.log(visualDataModel.model)
                        console.log(listView.model)
                        console.log(bodegaClient.session.installJobsModel)
                        if (visualDataModel.model.canFetchMore == undefined) {
                            return;
                        }

                        if (listView.atYEnd) {
                            loaderFooter.visible = visualDataModel.model.canFetchMore(rootIndex)
                        } else {
                            loaderFooter.visible = false
                        }
                    }
                }
                PlasmaComponents.BusyIndicator {
                    running: parent.visible
                    anchors.centerIn: parent
                }
            }

            header: customHeader
        }
    }

    PlasmaComponents.ToolButton {
        iconSource: "go-top"
        z: 100
        width: theme.largeIconSize
        height: width
        anchors {
            top: parent.top
            right: parent.right
            margins: 16
        }
        opacity: listView.flicking && listView.moving ? 1 : 0
        Behavior on opacity {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
        onClicked: PropertyAnimation {
            target: listView
            properties: "contentY"
            to: 0
            duration: 250
        }
    }
}
