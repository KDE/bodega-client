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
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
//import org.kde.qtextracomponents 0.1

Item {
    id: root
    property Item visualParent
    opacity: 1
    z: 9000
    property int status: PlasmaComponents.DialogStatus.Closed
    default property alias data: contentItem.data

    function open()
    {
        root.status = PlasmaComponents.DialogStatus.Opening
        appearAnimation.running = true
    }

    function close()
    {
        root.status = PlasmaComponents.DialogStatus.Closing
        appearAnimation.running = true
    }

    //TODO: support multiple directions
    x: internal.parentPos.x - internal.width/2 + root.visualParent.width/2
    y: internal.parentPos.y + visualParent.height + tipSvg.height

    SequentialAnimation {
        id: appearAnimation
        NumberAnimation {
            duration: 250
            easing.type: Easing.InOutQuad
            target: root
            properties: "opacity"
            to: root.status == PlasmaComponents.DialogStatus.Opening ? 1 : 0
        }
        ScriptAction {
            script: root.status == PlasmaComponents.DialogStatus.Opening ? root.status = PlasmaComponents.DialogStatus.Open : PlasmaComponents.DialogStatus.Closed
        }
    }

    PlasmaCore.FrameSvgItem {
        id: internal
        property variant parentPos: root.parent.mapToItem(root.parent, root.visualParent.x, root.visualParent.y)
        imagePath: "dialogs/background"
        width: contentItem.width + margins.left + margins.right
        height: contentItem.height + margins.top + margins.bottom

        PlasmaCore.SvgItem {
            id: tipSvg
            svg: PlasmaCore.Svg {
                id: backgroundSvg
                imagePath: "dialogs/background"
            }
            elementId: "baloon-tip-top"
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.top
                bottomMargin: -backgroundSvg.elementSize("hint-top-shadow").height

            }
            width: naturalSize.width
            height: naturalSize.height
        }
        Item {
            id: contentItem
            x: parent.margins.left
            y: parent.margins.top
            width: childrenRect.width
            height: childrenRect.height
        }
    }
    MouseArea {
        id: dismissArea
        z: 9000
        anchors.fill: parent
        Rectangle {
            anchors.fill: parent
            color: "#00000020"
        }
        onClicked: {
            root.close()
        }
        Component.onCompleted: {
            var candidate = root
            while (root.parent) {
                candidate = candidate.parent
            }
            if (candidate) {
                dismissArea.parent = candidate
            } else {
                dismissArea.visible = false
            }
        }
    }
}
