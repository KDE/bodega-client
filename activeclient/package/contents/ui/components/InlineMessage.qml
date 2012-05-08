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

Baloon {
    id: root
    property alias title: titleLabel.text
    property alias message: messageLabel.text

    Column {
        spacing: 4
        width: theme.defaultFont.mSize.width * 22
        anchors {
            left: parent.left
            right: parent.right
        }
        PlasmaComponents.Label {
            id: titleLabel
            anchors {
                left: parent.left
                right: parent.right
            }
            elide: Text.elideLeft
            horizontalAlignment: Text.AlignHCenter
        }
        PlasmaComponents.Label {
            id: messageLabel
            anchors {
                left: parent.left
                right: parent.right
            }
            wrapMode: Text.Wrap
        }
    }
}
