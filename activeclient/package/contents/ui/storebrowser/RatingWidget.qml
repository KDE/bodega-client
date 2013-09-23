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
import org.kde.plasma.mobilecomponents 0.1 as MobileComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.qtextracomponents 0.1
import "../components"

Column {
    spacing: theme.defaultFont.mSize.height

    PlasmaComponents.Button {
        id: ratingsButton
        anchors.horizontalCenter: parent.horizontalCenter
        text: i18n("Rate and review")
        onClicked: ratingsBaloon.open()
    }

    MouseArea {
        anchors {
            left: parent.left
            right: parent.right
        }

        visible: ratingsRepeater.model.ratingsCount > 0
        height: childrenRect.height

        onClicked: {
            itemBrowser.pop(root)
            bodegaClient.assetRatingsJobModel.assetId = assetId;
            itemBrowser.push(Qt.resolvedUrl("RatingsColumn.qml"))
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
            }
            spacing: 4
            PlasmaComponents.Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18n("Average of %1 user ratings:", ratingsRepeater.model.ratingsCount)
            }


            Repeater {
                id: ratingsRepeater
                model: assetOperations.ratingsModel
                delegate: Row {
                    visible: model.RatingsCount > 0
                    PlasmaComponents.Label {
                        id: attributeNameLabel
                        text: i18n("%1: ", model.Name)
                        wrapMode: Text.Wrap
                        width: theme.defaultFont.mSize.width*8
                        horizontalAlignment: Text.AlignRight
                    }
                    RatingStars {
                        rating: AverageRating
                        starSize: theme.defaultFont.mSize.height * 1.6
                        enabled: false
                    }
                }
            }
            PlasmaComponents.Label {
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18n("See all ratings...")
                color: theme.linkColor
            }
        }
    }
    PlasmaComponents.Label {
        visible: ratingsRepeater.model.ratingsCount == 0
        anchors.horizontalCenter: parent.horizontalCenter
        text: i18n("No user ratings yet.")
    }

    Baloon {
        id: ratingsBaloon
        visualParent: ratingsButton
        property variant ratingAttributes: {}
        Column {
            spacing: 5
            Repeater {
                model: assetOperations.ratingsModel
                delegate: Row {
                    anchors.right: parent.right
                    spacing: 5
                    PlasmaComponents.Label {
                        anchors.verticalCenter: stars.verticalCenter
                        text: i18n("%1: ", model.Name)
                    }

                    RatingStars {
                        id: stars
                        starSize: theme.defaultFont.mSize.height * 2
                        rating: model.RatingValue
                        onRatingChanged: {
                            var tmp = ratingsBaloon.ratingAttributes;
                            tmp[model.AttributeId] = rating;
                            ratingsBaloon.ratingAttributes = tmp;
                        }
                    }
                }
            }

            PlasmaComponents.Button {
                text: i18n("Ok")
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    ratingsBaloon.close()
                    var job = bodegaClient.session.assetCreateRatings(assetId, ratingsBaloon.ratingAttributes)
                    job.jobFinished.connect(root.reloadPage)
                    job.jobFinished.connect(bodegaClient.participantRatingsJobModel.reload)
                }
            }

        }
    }
}