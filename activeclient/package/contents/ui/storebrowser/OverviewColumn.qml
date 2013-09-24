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
import org.kde.plasma.extras 0.1 as PlasmaExtras
import "../components"

BrowserColumn {
    id: root
    clip: true

    //FIXME: why it needs delay to work correctly?
    Timer {
        interval: 10
        running: true
        onTriggered: {
            if (bodegaClient.startPage === "updates") {
                categoriesColumn.currentIndex = updatesItem.index;
                itemBrowser.push(Qt.createComponent("UpdatesColumn.qml"));
            }
        }
    }

    PlasmaExtras.ScrollArea {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: statusFrame.top
        }
        //It's a flickable+repeater because we need to mix other items to the model
        //results should be small enough to be loaded all at once
        Flickable {
            id: categoriesFlickable
            interactive: height < contentHeight
            anchors.fill: parent
            contentWidth: categoriesColumn.width
            contentHeight: categoriesColumn.height
            Column {
                id: categoriesColumn
                property int currentIndex: -1
                width: categoriesFlickable.width

                Repeater {
                    model: VisualDataModel {
                        id: visualDataModel
                        model: bodegaClient.channelsModel
                        delegate: StoreListItem {
                            checked: categoriesColumn.currentIndex == index
                            onClicked: {
                                if (categoriesColumn.currentIndex == index) {
                                    return
                                }
                                categoriesColumn.currentIndex = index
                                itemBrowser.pop(root)
                                var channels = itemBrowser.push(Qt.createComponent("ChannelsColumn.qml"))
                                channels.rootIndex = visualDataModel.modelIndex(index)
                                channels.channelId = model.ChannelIdRole
                            }
                        }
                    }
                }
                StoreListItem {
                    id: collectionsItem
                    visible: count > 0
                    label: i18n("Collections")
                    property int index: visualDataModel.count
                    count: bodegaClient.listCollectionsJobModel.count
                    checked: categoriesColumn.currentIndex == index
                    onClicked: {
                        if (categoriesColumn.currentIndex == index) {
                            return
                        }
                        categoriesColumn.currentIndex = index
                        itemBrowser.pop(root);
                        var collections = itemBrowser.push(Qt.createComponent("ListCollectionsJobColumn.qml"))
                    }
                }
                StoreListItem {
                    id: updatesItem
                    visible: count > 0
                    icon: "system-software-update"
                    label: i18n("Updates")
                    property int index: collectionsItem.index + 1
                    count: bodegaClient.updatedAssetsModel.count
                    checked: categoriesColumn.currentIndex == index
                    onClicked: {
                        if (categoriesColumn.currentIndex == index) {
                            return
                        }
                        categoriesColumn.currentIndex = index;
                        itemBrowser.pop(root);
                        var page = itemBrowser.push(Qt.createComponent("UpdatesColumn.qml"));
                    }
                }
                StoreListItem {
                    id: downloadsItem
                    visible: count > 0
                    icon: "folder-downloads"
                    label: i18n("Downloads")
                    property int index: updatesItem.index + 1
                    count: bodegaClient.session.installJobsModel.count
                    checked: categoriesColumn.currentIndex == index
                    onClicked: {
                        if (categoriesColumn.currentIndex == index) {
                            return
                        }
                        categoriesColumn.currentIndex = index
                        itemBrowser.pop(root)
                        var channels = itemBrowser.push(Qt.createComponent("InstallJobsColumn.qml"))
                    }
                }
            }
        }
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
