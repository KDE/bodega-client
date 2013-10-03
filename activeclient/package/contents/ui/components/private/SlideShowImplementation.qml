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

Rectangle {
    id: root
    property alias model: listView.model
    property alias currentIndex: listView.currentIndex
    signal clicked(int index)

    color: "black"

    ListView {
        id: listView
        anchors.fill: parent
        snapMode: ListView.SnapToItem
        clip: true
        orientation: ListView.Horizontal
        highlightRangeMode: ListView.StrictlyEnforceRange
        delegate: Image {
            width: listView.width
            height: listView.height
            source: fileName
            fillMode: Image.PreserveAspectFit
            MouseArea {
                anchors.fill: parent
                onClicked: root.clicked(index)
            }
        }
    }
    Row {
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 4
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

