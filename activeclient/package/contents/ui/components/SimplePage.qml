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


/*This is a simple page with the logo on top and a small form in the middle*/

PlasmaComponents.Page {
    id: root
    function showMessage(title, message)
    {
        messageBox.title = title
        messageBox.text = message
        messageBox.state = "right"
        messageBox.state = ""
    }
    function hideMessage()
    {
        messageBox.state = "left"
    }

    property bool titleShown: true
    property alias title: titleLabel.text
    default property alias content: contentRect.data

    Image {
        id: logo
        width: 556
        height: 50
        visible: titleShown
        source: viewerPackage.filePath("images", "makeplaylive.png")
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: theme.defaultFont.mSize.height
        }
    }

    PlasmaComponents.Label {
        id: titleLabel
        text: i18n("add-ons and content store")
        visible: titleShown
        anchors {
            top: logo.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }

    Item {
        id: contentRect
        anchors {
            top: titleShown ? titleLabel.bottom : parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

    PlasmaCore.FrameSvgItem {
        id: messageBox
        z: 9999
        property alias title: messageTitleLabel.text
        property alias text: messageLabel.text
        state: "right"

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: theme.defaultFont.mSize.height * 2
        }

        imagePath: "widgets/frame"
        prefix: "raised"
        width: parent.width/3
        height: childrenRect.height + margins.top + margins.bottom
        Column {
            id: mainColumn
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                leftMargin: parent.margins.left
                rightMargin: parent.margins.right
                topMargin: parent.margins.top
            }
            PlasmaComponents.Label {
                id: messageTitleLabel
                anchors.horizontalCenter: parent.horizontalCenter
            }
            PlasmaComponents.Label {
                id: messageLabel

                wrapMode: Text.WordWrap
                horizontalAlignment: paintedHeight > theme.defaultFont.mSize.height ? Text.AlignJustify : Text.AlignHCenter
                anchors {
                    left: parent.left
                    right: parent.right
                }
            }
        }
        MouseArea {
            anchors.fill: mainColumn
            onClicked: root.hideMessage()
        }

        transform: Translate {
            id: translateTransform
        }
        states: [
            State {
                name: ""
                PropertyChanges {
                    target: translateTransform
                    x: 0
                }

            },
            State {
                name: "right"
                PropertyChanges {
                    target: translateTransform
                    x: root.width - messageBox.x
                }
            },
            State {
                name: "left"
                PropertyChanges {
                    target: translateTransform
                    x: - messageBox.x - messageBox.width
                }
            }
        ]
        transitions: [
            Transition {
                from: "right"
                to: ""
                NumberAnimation {
                    targets: translateTransform
                    properties: "x"
                    duration: 250
                    easing.type: "OutQuad"
                }
            },
            Transition {
                from: ""
                to: "left"
                NumberAnimation {
                    targets: translateTransform
                    properties: "x"
                    duration: 250
                    easing.type: "InQuad"
                }
            }
        ]
    }
}
