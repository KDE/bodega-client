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

    property variant rootIndex
    property string channelId

    VisualDataModel {
        id: channelsModel
        model: bodegaClient.session.installJobsModel
        rootIndex: root.rootIndex
        delegate: delegateComponent
    }


    ListView {
        id: categoriesView
        currentIndex: -1
        clip: true
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        model: channelsModel

        delegate: Component {
            id: delegateComponent
            PlasmaComponents.ListItem {
                id: listItem
                enabled: true
                checked: categoriesView.currentIndex == index
                Row {
                    id: delegateRow
                    spacing: theme.defaultFont.mSize.width
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    Image {
                        id: iconImage
                        source: model.ImageMediumRole
                        width: theme.mediumIconSize
                        height: width
                    }
                    Column {
                        PlasmaComponents.Label {
                            text: model.DisplayRole
                            width: delegateRow.width - iconImage.width - theme.defaultFont.mSize.width
                            elide: Text.ElideRight
                        }
                        PlasmaComponents.ProgressBar {
                            id: downloadProgress
                            visible: value < maximumValue
                            minimumValue: 0
                            maximumValue: 100
                            value: model.ProgressRole*100
                            height: doneLabel.height
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                        }
                        PlasmaComponents.Label {
                            id: doneLabel
                            text: i18n("Download finished")
                            height: paintedHeight
                            visible: !downloadProgress.visible
                            font.pointSize: theme.smallestFont.pointSize
                        }
                    }
                }
                onClicked: {
                    if (categoriesView.currentIndex == index) {
                        return
                    }
                    categoriesView.currentIndex = index
                    itemBrowser.pop(root)

                    if (model.AssetIdRole) {
                        var assetPage = itemBrowser.push(Qt.createComponent("AssetColumn.qml"))
                        assetPage.assetId = model.AssetIdRole
                    }
                }
            }
        }

        footer: PlasmaComponents.ListItem {
            id: loaderFooter
            Connections {
                target: categoriesView
                onAtYEndChanged: {
                    if (categoriesView.atYEnd) {
                        loaderFooter.visible = bodegaClient.channelsModel.canFetchMore(rootIndex)
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
    PlasmaComponents.ScrollBar {
        id: scrollBar
        z: 800
        flickableItem: categoriesView
        orientation: Qt.Vertical
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