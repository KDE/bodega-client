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
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.qtextracomponents 0.1
import "../components"


PlasmaComponents.Page {
    id: root

    PlasmaComponents.Label {
        text: i18n("Loading ratings...")
        anchors.centerIn: parent
        visible: listView.count == 0
        PlasmaComponents.BusyIndicator {
            running: visible
            height: parent.height
            width: height
            anchors.right: parent.left
        }
    }

    PlasmaExtras.ScrollArea {
        anchors.fill: parent
        ListView {
            id: listView

            anchors.fill: parent

            model: bodegaClient.participantRatingsJobModel

            delegate: PlasmaComponents.ListItem {
                Column {
                    spacing: 0
                    PlasmaComponents.Label {
                        text: model.AssetId
                        wrapMode: Text.Wrap
                        width: root.width
                        visible: text.length > 0
                    }

                    PlasmaComponents.Label {
                        text: model.AttributeId
                        wrapMode: Text.Wrap
                        width: root.width
                        visible: text.length > 0
                    }

                    PlasmaComponents.Label {
                        text: model.Rating
                        visible: text.length > 0
                    }
                }
            }
        }
    }
}
