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
    id: widgetRoot
    property bool expanded: false
    spacing: theme.defaultFont.mSize.height

    onExpandedChanged: {
        mainFlickable.interactive = !expanded
    }
    PlasmaComponents.Button {
        id: ratingsButton
        z: 100
        anchors.horizontalCenter: parent.horizontalCenter
        text: i18n("Rate and review")
        onClicked: ratingsBaloon.open()
    }

    MouseArea {
        id: ratingsExpander
        anchors {
            left: parent.left
            right: parent.right
        }

        visible: ratingsRepeater.model.ratingsCount > 0
        height: starsColumn.height

        onClicked: {
            bodegaClient.assetRatingsJobModel.assetId = assetId;
            widgetRoot.expanded = !widgetRoot.expanded
        }

        Rectangle {
            anchors {
                fill: parent
                leftMargin: -theme.defaultFont.mSize.width
                rightMargin: -theme.defaultFont.mSize.width
                topMargin: -theme.defaultFont.mSize.height*2 -ratingsButton.height -1
                bottomMargin: -theme.defaultFont.mSize.height -1
            }
            color: theme.textColor
            opacity: 0.1
            visible: parent.pressed
        }
        Column {
            id: starsColumn
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
                text: widgetRoot.expanded ? i18n("Hide ratings") : i18n("See all ratings...")
                color: theme.linkColor
            }
        }
    }
    PlasmaComponents.Label {
        visible: ratingsRepeater.model.ratingsCount == 0
        anchors.horizontalCenter: parent.horizontalCenter
        text: i18n("No user ratings yet.")
    }

    PlasmaExtras.ConditionalLoader {
        id: ratingsColumnLoader
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: -theme.defaultFont.mSize.width
            rightMargin: -theme.defaultFont.mSize.width
        }
        height: expanded ? scrollArea.height - ratingsExpander.height - ratingsButton.height -theme.defaultFont.mSize.height*3  : 0
        when: widgetRoot.expanded
        source: Qt.resolvedUrl("RatingsColumn.qml")
        Behavior on height {
            SequentialAnimation {
                NumberAnimation {
                    duration: 250
                    easing.type: Easing.InOutQuad
                }
                ScriptAction {
                    script: {
                        if (widgetRoot.expanded) {
                            mainFlickable.contentY = widgetRoot.y-theme.defaultFont.mSize.height
                        }
                    }
                }
            }
        }
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