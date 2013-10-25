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
import org.kde.qtextracomponents 0.1

Item {
    id: root
    property alias model: listView.model
    property alias currentIndex: listView.currentIndex
    property bool showBackround: true
    signal clicked(int index)
    implicitHeight: roundSize(width)

    function imageforSize(width) {
        if (width < 32) {
            return 'tiny';
        } else if (width < 64) {
            return 'small';
        } else if (width < 128) {
            return 'medium';
        } else if (width < 256) {
            return 'big';
        } else if (width < 512) {
            return 'large';
        } else {
            return 'huge';
        }
    }
    function roundSize(width) {
        if (width < 32) {
            return 22;
        } else if (width < 64) {
            return 32;
        } else if (width < 128) {
            return 64;
        } else if (width < 256) {
            return 128;
        } else if (width < 512) {
            return 256;
        } else {
            return 512;
        }
    }

    Component {
        id: iconDelegate
        Item {
            property QtObject model
            width: listView.width
            height: listView.height
            Image {
                anchors.centerIn: parent
                width: roundSize(Math.min(parent.width, parent.height))
                height: width
                source: model[imageforSize(width)]
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    anchors.fill: parent
                    onClicked: root.clicked(model.index)
                }
            }
        }
    }
    Component {
        id: previewDelegate
        Item {
            property QtObject model
            width: listView.width
            height: listView.height
            PlasmaCore.FrameSvgItem {
                id: photoBackground
                width: screenshot.paintedWidth ? screenshot.paintedWidth + margins.left + margins.right : parent.width
                height: screenshot.paintedHeight ? screenshot.paintedHeight + margins.top + margins.bottom : parent.height
                anchors.centerIn: parent
                anchors.verticalCenter: parent.verticalCenter
                imagePath: showBackround ? "widgets/media-delegate" : ''
                prefix: "picture"
            }
            Rectangle {
                id: blackFrame
                anchors {
                    fill: photoBackground
                    leftMargin: photoBackground.margins.left
                    topMargin: photoBackground.margins.top
                    rightMargin: photoBackground.margins.right
                    bottomMargin: photoBackground.margins.bottom
                }
                color: 'black'
            }
            Image {
                id: screenshot
                anchors {
                    fill: parent
                    leftMargin: photoBackground.margins.left
                    topMargin: photoBackground.margins.top
                    rightMargin: photoBackground.margins.right
                    bottomMargin: photoBackground.margins.bottom
                }
                source: model.fileName
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    anchors.fill: parent
                    onClicked: root.clicked(model.index)
                }
            }
        }
    }
    ListView {
        id: listView
        anchors.fill: parent
        snapMode: ListView.SnapToItem
        cacheBuffer: 10
        clip: true
        orientation: ListView.Horizontal
        highlightRangeMode: ListView.StrictlyEnforceRange
        delegate: Loader {
            sourceComponent: model.type == 'icon' ? iconDelegate : previewDelegate
            Component.onCompleted: {
                item.model = model      
            }
        }
    }
    Row {
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: parent.height/10
        }
        visible: listView.count > 1
        spacing: 2
        Repeater {
            model: listView.count
            delegate: Item {
                width: 12
                height: 12
                Rectangle {
                    color: theme.backgroundColor
                    radius: width
                    smooth: true
                    opacity: 0.8
                    border {
                        color: theme.textColor
                        width: 1
                    }
                    anchors.centerIn: parent
                    width: index == listView.currentIndex ? parent.width : (parent.width/3)*2
                    height: width
                    Behavior on width {
                        NumberAnimation {
                            duration: 250
                            easing.type: Easing.InOutQuad
                        }
                    }
                }
            }
        }
    }
    Timer {
        running: true
        interval: 8000
        repeat: true
        onTriggered: listView.currentIndex = (listView.currentIndex+1)%listView.count
    }
}

