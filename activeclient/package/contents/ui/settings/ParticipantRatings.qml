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

            model: bodegaClient.participantRatingsModel

            delegate: PlasmaComponents.ListItem {
                id: listItem
                property variant ratings: model.Ratings
                Column {
                    width: parent.width

                    PlasmaExtras.Heading {
                        level: 2
                        text: model.AssetName
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                    }
                    Row {
                        x: Math.max(0, parent.width / 2 - theme.defaultFont.mSize.width * 11)
                        spacing: 4
                        PlasmaComponents.Label {
                            text: i18n("Description:")
                            wrapMode: Text.Wrap
                            width: theme.defaultFont.mSize.width*11
                            horizontalAlignment: Text.AlignRight
                            verticalAlignment: Text.AlignTop
                            visible: model.AssetDesciption.length > 0
                        }
                        PlasmaComponents.Label {
                            id: descriptionLabel
                            text:  model.AssetDesciption
                            width: listItem.width/2
                            visible: text.length > 0
                            verticalAlignment: Text.AlignTop
                            wrapMode: Text.Wrap
                            elide: Text.ElideLeft
                            maximumLineCount: 3
                        }
                    }
                    Row {
                        x: Math.max(0, parent.width/2 - theme.defaultFont.mSize.width * 11)
                        spacing: 4
                        PlasmaComponents.Label {
                            text: i18n("Version:")
                            wrapMode: Text.Wrap
                            width: theme.defaultFont.mSize.width * 11
                            horizontalAlignment: Text.AlignRight
                            visible: model.AssetVersion.length > 0
                        }
                        PlasmaComponents.Label {
                            text:  model.AssetVersion
                            width: theme.defaultFont.mSize.width * 11
                            visible: text.length > 0
                        }
                    }

                    Repeater {
                        model: ratings.length
                        delegate: Row {
                            id: ratingsRow
                            x: Math.max(0, parent.width / 2 - theme.defaultFont.mSize.width * 11)
                            spacing: 4
                            PlasmaComponents.Label {
                                id: ratingsLabel
                                text: i18n("%1 :", ratings[index]["AttributeName"])
                                wrapMode: Text.Wrap
                                width: theme.defaultFont.mSize.width*11
                                horizontalAlignment: Text.AlignRight
                            }

                            RatingStars {
                                rating: ratings[index]["Rating"]
                                starSize: theme.defaultFont.mSize.height * 1.6
                                enabled: false
                            }
                        }
                    } // end of Repeater


                    PlasmaComponents.Button {
                        id: ratingsDeleteButton
                        x: listItem.width / 2
                        text: i18n("Delete this rating")
                        onClicked: ratingsDeleteConfirmation.open()
                    }
                    Item {
                        width: 1
                        height: theme.defaultFont.mSize.height/4
                    }

                    InlineConfirmationDialog {
                        id: ratingsDeleteConfirmation
                        visualParent: ratingsDeleteButton
                        message: i18n("Are you sure you want to remove all your ratings from the asset?")
                        onAccepted: {
                            var job = bodegaClient.session.assetDeleteRatings(model.AssetId)
                            job.jobFinished.connect(bodegaClient.participantRatingsModel.reload)
                        }
                    }
                }
            }
        }
    }
}
