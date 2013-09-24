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

PlasmaComponents.ListItem {
    id: root
    enabled: true
    property int iconSize: theme.mediumIconSize
    property string icon
    property string label: model.DisplayRole
    property int count: model.ChannelAssetCountRole ? model.ChannelAssetCountRole : -1
    Row {
        spacing: theme.defaultFont.mSize.width
        anchors {
            left: parent.left
            right: parent.right
        }
        Loader {
            id: iconLoader
            width: iconSize
            height: iconSize
            sourceComponent: (root.icon || model.DecorationRole) ? qIconComponent : (model.ImageMediumRole ? iconComponent : emptyComponent)
            anchors.verticalCenter: parent.verticalCenter
        }
        PlasmaComponents.Label {
            id: label
            text: root.label
            anchors {
                left: iconLoader.right
                leftMargin: 4
                verticalCenter: parent.verticalCenter
                right: countLabel.left
            }
            width: parent.width - iconLoader.width - theme.defaultFont.mSize.width
            elide: Text.ElideRight
        }
        PlasmaComponents.Label {
            id: countLabel
            text: root.count >= 0 ? root.count : ''
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
            }
        }
    }
    Component {
        id: iconComponent
        Image {
            source: model.ImageMediumRole
            anchors.fill: parent
        }
    }
    Component {
        id: qIconComponent
        QIconItem {
            icon: root.icon
            anchors.fill: parent
        }
    }
    Component {
        id: emptyComponent
        Rectangle {
            color: theme.highlightColor
            radius: anchors.margins
            anchors {
                fill: parent
                margins: iconSize/5
            }
        }
    }
}
