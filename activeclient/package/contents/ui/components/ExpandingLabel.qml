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

MouseArea {
    id: root

    property alias text: descriptionLabel.text
    property bool expanded: false
    anchors {
        left: parent.left
        right: parent.right
    }

    height: mainColumn.height
    onClicked: root.expanded = !root.expanded
    enabled: (!expanded && descriptionLabel.paintedHeight > descriptionLabel.height)

    Column {
        id: mainColumn

        anchors {
            left: parent.left
            right: parent.right
        }
        PlasmaComponents.Label {
            id: descriptionLabel
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignJustify
            height: root.expanded ? paintedHeight : theme.defaultFont.mSize.height * 4
            clip: true
            anchors {
                left: parent.left
                right: parent.right
            }

            Behavior on height {
                NumberAnimation {
                    duration: 250
                    easing.type: Easing.InOutQuad
                }
            }
        }
        
        PlasmaCore.SvgItem {
            svg: PlasmaCore.Svg{
                imagePath: "widgets/arrows"
            }
            visible: root.enabled
            elementId: expanded ? "up-arrow" : "down-arrow"
            height: naturalSize.width
            width: naturalSize.width
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
