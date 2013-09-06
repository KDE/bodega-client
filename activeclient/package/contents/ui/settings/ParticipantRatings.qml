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
            currentIndex: -1
            anchors.fill: parent

            model: bodegaClient.participantRatingsJobModel

            delegate: PlasmaComponents.ListItem {
                Column {
                    spacing: 0
                    PlasmaComponents.Label {
                        text: model.AssetName
                        wrapMode: Text.Wrap
                        width: root.width
                        visible: text.length > 0
                    }
                    PlasmaComponents.Label {
                        text: i18n("Description: %1", model.AssetDesciption)
                        wrapMode: Text.Wrap
                        width: root.width
                        visible: text.length > 0
                    }
                    PlasmaComponents.Label {
                        text: i18n("Version: %1", model.AssetVersion)
                        wrapMode: Text.Wrap
                        width: root.width
                        visible: text.length > 0
                    }

                    PlasmaComponents.Label {
                        text: i18n("Rating: %1", model.AttributeName)
                        wrapMode: Text.Wrap
                        width: root.width
                        visible: text.length > 0
                    }

                    PlasmaComponents.Label {
                        text: i18n("Rating Average: %1", model.Rating)
                        visible: text.length > 0
                    }
                    PlasmaComponents.Button {
                        id: ratingsDeleteButton
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: i18n("Delete ratings")
                        onClicked: ratingsDeleteConfirmation.open()
                    }

                    InlineConfirmationDialog {
                        id: ratingsDeleteConfirmation
                        visualParent: ratingsDeleteButton
                        message: i18n("Are you sure you want to remove all your ratings from the asset")
                        onAccepted: {
                            var job = bodegaClient.session.assetDeleteRatings(model.AssetId)
                            job.jobFinished.connect(root.reloadPage)
                        }
                    }
                }
            }
        }
    }
}
