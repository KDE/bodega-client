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

PlasmaComponents.Page {
    id: root
    width: 300
    height: 300

    Flickable {
        id: mainFlickable
        anchors.fill: parent
        contentHeight: height
        contentWidth: contentItem.width
        boundsBehavior: Flickable.StopAtBounds
        Item {
            id: contentItem
            height: parent.height
            width: Math.max(root.width, sideBar.width * 2)
            NumberAnimation {
                id: scrollAnimation
                target: mainFlickable
                properties: "contentX"
                duration: 250
                easing.type: Easing.InOutQuad
            }
            SimplePage {
                id: sideBar
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }
                width: root.width/Math.round(root.width/(theme.defaultFont.mSize.width*30))
                title: "Make Play Live"
                //FIXME: why necessary?
                visible: true

                ListView {
                    id: categoriesView
                    clip: true
                    currentIndex: 2
                    anchors {
                        top: parent.top
                        bottom: toolBar.top
                        left: parent.left
                        right: parent.right
                    }
                    model: ListModel {
                        ListElement { DisplayRole: "Payment methods"; component: "PaymentMethods" }
                        ListElement { DisplayRole: "Account history"; component: "AccountHistory" }
                        ListElement { DisplayRole: "Personal data"; component: "PersonalData" }
                        ListElement { DisplayRole: "Add points"; component: "AddPoints" }
                    }
                    delegate: StoreListItem {
                        checked: categoriesView.currentIndex == index
                        property string title: label
                        onClicked: {
                            if (categoriesView.currentIndex == index) {
                                return
                            }
                            settingsStack.replace(Qt.createComponent("settings/" + component + ".qml"))
                            categoriesView.currentIndex = index

                            if (contentItem.width > mainFlickable.width) {
                                scrollAnimation.to = sideBar.width
                                scrollAnimation.running = true
                            }
                        }
                    }
                }
                PlasmaComponents.ScrollBar {
                    flickableItem: categoriesView
                    orientation: Qt.Vertical
                }

                PlasmaComponents.ToolBar {
                    id: toolBar
                    anchors.bottom: parent.bottom

                    tools: PlasmaComponents.ToolBarLayout {
                        PlasmaComponents.ToolButton {
                            id: backButton
                            iconSource: "go-previous"
                            width: theme.largeIconSize
                            height: width
                            flat: false
                            onClicked: mainStack.pop()
                        }
                        PlasmaComponents.Button {
                            text: i18n("Disconnect account")
                            anchors.verticalCenter: parent.verticalCenter
                            onClicked: {
                                //make kwallet forget
                                bodegaClient.saveCredentials("", "")
                                bodegaClient.session.signOut()
                                mainStack.pop(mainStack.initialPage)
                            }
                        }
                    }
                }
            }

            Image {
                source: "image://appbackgrounds/standard"
                fillMode: Image.Tile

                PlasmaComponents.Label {
                    id: titleLabel
                    anchors {
                        right: parent.right
                        top: parent.top
                        margins: 10
                    }
                    text: categoriesView.currentItem.title
                    font.pointSize: theme.defaultFont.pointSize * 1.5
                }

                anchors {
                    left: sideBar.right
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                PlasmaComponents.PageStack {
                    id: settingsStack
                    anchors {
                        top: titleLabel.bottom
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                        margins: 8
                    }
                    clip: true
                    initialPage: Qt.createComponent("settings/PersonalData.qml")
                }
                Image {
                    z: 800
                    source: "image://appbackgrounds/shadow-left"
                    fillMode: Image.TileVertically
                    anchors {
                        right: parent.left
                        top: parent.top
                        bottom: parent.bottom
                    }
                }
            }
        }
    }
    PlasmaComponents.ScrollBar {
        flickableItem: mainFlickable
        orientation: Qt.Horizontal
    }
}