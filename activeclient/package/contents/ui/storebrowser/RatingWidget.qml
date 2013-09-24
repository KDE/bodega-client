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

        PlasmaCore.IconItem {
            id: successIcon
            visible: false
            anchors {
                left: parent.right
                leftMargin: 4
            }
            source: "dialog-ok-apply"
        }
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
        state: widgetRoot.expanded ? "expanded" : "collapsed"
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: -theme.defaultFont.mSize.width
            rightMargin: -theme.defaultFont.mSize.width
        }
        visible: false
        when: widgetRoot.expanded
        source: Qt.resolvedUrl("RatingsColumn.qml")

        states: [
            State {
                name: "collapsed"
                PropertyChanges {
                    target: ratingsColumnLoader
                    height: 0
                }
            },
            State {
                name: "expanded"
                PropertyChanges {
                    target: ratingsColumnLoader
                    height: scrollArea.height - ratingsExpander.height - ratingsButton.height -theme.defaultFont.mSize.height*3
                }
            }
        ]
        transitions: [
            Transition {
                from: "collapsed"
                to: "expanded"
                SequentialAnimation {
                    ScriptAction {
                        script: ratingsColumnLoader.visible = true
                    }
                    NumberAnimation {
                        targets: ratingsColumnLoader
                        properties: "height"
                        duration: 150
                        easing.type: Easing.InOutQuad
                    }
                    NumberAnimation {
                        targets: mainFlickable
                        properties: "contentY"
                        to: widgetRoot.y-theme.defaultFont.mSize.height
                        duration: 150
                        easing.type: Easing.InOutQuad
                    }
                }
            },
            Transition {
                from: "expanded"
                to: "collapsed"
                SequentialAnimation {
                    NumberAnimation {
                        targets: ratingsColumnLoader
                        properties: "height"
                        duration: 150
                        easing.type: Easing.InOutQuad
                    }
                    NumberAnimation {
                        targets: mainFlickable
                        properties: "contentY"
                        to: 0
                        duration: 150
                        easing.type: Easing.InOutQuad
                    }
                    ScriptAction {
                        script: ratingsColumnLoader.visible = false
                    }
                }
            }
        ]
    }
    Baloon {
        id: ratingsBaloon
        visualParent: ratingsButton
        property variant ratingAttributes: {}

        //have to use Connections in order for job to be uderstood by QML
        Connections {
            id: ratingConnections
            onJobFinished: {
                ratingsBaloon.close();
                successIcon.visible = !job.failed;
                ratingBusyWidget.visible = false;
                root.reloadPage();
                bodegaClient.participantRatingsJobModel.reload();
                hideTimer.restart();
            }
            onJobError: {
                ratingsBaloon.close();
                showMessage(error.title, error.description, ratingsButton)
            }
        }


        Timer {
            id: hideTimer
            triggeredOnStart: false
            interval: 2000
            running: false
            repeat: false
            onTriggered: {
                successIcon.visible = false;
            }
        }
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
                        rating: 0
                        onRatingChanged: {
                            var tmp = ratingsBaloon.ratingAttributes;
                            tmp[model.AttributeId] = rating;
                            ratingsBaloon.ratingAttributes = tmp;
                            rateConfirmButton.enabled = true;
                        }
                    }
                }
            }

            PlasmaComponents.Button {
                id: rateConfirmButton
                text: i18n("Rate")
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: false
                onClicked: {
                    ratingBusyWidget.visible = true;
                    ratingConnections.target = bodegaClient.session.assetCreateRatings(assetId, ratingsBaloon.ratingAttributes);
                }
            }

        }
        PlasmaComponents.BusyIndicator {
            id: ratingBusyWidget
            anchors.centerIn: parent
            visible: false
            running: visible
        }

    }
}