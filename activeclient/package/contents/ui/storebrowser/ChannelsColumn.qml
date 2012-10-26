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

    property variant rootIndex
    property string channelId

    VisualDataModel {
        id: channelsModel
        model: bodegaClient.channelsModel
        rootIndex: root.rootIndex
        delegate: delegateComponent
    }
    VisualDataModel {
        id: searchModel
        model: bodegaClient.searchModel
        delegate: delegateComponent
    }

    PlasmaComponents.ToolBar {
        id: toolBar
        y: -categoriesView.contentY
        z: 1
        tools: MobileComponents.ViewSearch {
            id: searchField
            onSearchQueryChanged: {
                //var job = bodegaClient.session.search(searchQuery, -1, -1)
                if (searchQuery.length > 3) {
                    categoriesView.model = searchModel
                    bodegaClient.searchModel.topChannel = root.channelId
                    bodegaClient.searchModel.searchQuery = searchQuery
                } else {
                    categoriesView.model = channelsModel
                    bodegaClient.searchModel.searchQuery = ""
                }
                categoriesView.currentIndex = -1
            }
        }
    }

    PlasmaExtras.ScrollArea {
        anchors.fill: parent
        
        ListView {
            id: categoriesView
            currentIndex: -1
            clip: true
            anchors.fill: parent

            model: channelsModel

            Component {
                id: delegateComponent
                StoreListItem {
                    checked: categoriesView.currentIndex == index
                    onClicked: {
                        if (categoriesView.currentIndex == index) {
                            return
                        }
                        categoriesView.currentIndex = index
                        itemBrowser.pop(root)

                        if (model.ChannelIdRole) {
                            var channelsPage = itemBrowser.push(Qt.createComponent("ChannelsColumn.qml"))
                            channelsPage.rootIndex = categoriesView.model.modelIndex(index)
                            channelsPage.channelId = model.ChannelIdRole
                        } else if (model.AssetIdRole) {
                            var assetPage = itemBrowser.push(Qt.createComponent("AssetColumn.qml"))
                            assetPage.assetId = model.AssetIdRole
                        }
                    }
                }
            }

            header: Item {
                width: parent.width
                height: toolBar.height
            }

            footer: PlasmaComponents.ListItem {
                id: loaderFooter
                Connections {
                    target: categoriesView
                    onAtYEndChanged: {
                        if (categoriesView.atYEnd) {
                            loaderFooter.visible = categoriesView.model.canFetchMore(rootIndex)
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
        opacity: categoriesView.flicking && categoriesView.moving ? 1 : 0
        Behavior on opacity {
            NumberAnimation {
                duration: 250
                easing.type: Easing.InOutQuad
            }
        }
        onClicked: PropertyAnimation {
            target: categoriesView
            properties: "contentY"
            to: 0
            duration: 250
        }
    }
}
