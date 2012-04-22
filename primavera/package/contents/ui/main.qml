/*
 *   Copyright 2012 Coherent Theory LLC
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
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.plasma.mobilecomponents 0.1 as MobileComponents
/*
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.qtextracomponents 0.1
*/

Image {
    id: primavera
    source: "image://appbackgrounds/contextarea"
    fillMode: Image.Tile

    width: 800
    height: 480

    MobileComponents.Package {
        id: files
        name: "com.coherenttheory.primavera"
    }

    transitions: [
       Transition {
            to: "hideGlyphs"
            SequentialAnimation {
                PauseAnimation { duration: 1500 }
                ParallelAnimation {
                PropertyAnimation { target: glyph1; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph2; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph3; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph4; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph5; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph6; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph7; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph8; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph9; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph10; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph11; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph12; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph13; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph14; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph15; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph16; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph17; properties: "width"; duration: 300; to: 0 }
                PropertyAnimation { target: glyph18; properties: "width"; duration: 300; to: 0 }
                }
                PauseAnimation { duration: 200 }
                ScriptAction { scriptName: "reloadGlyphs" }
            }
       },
       Transition {
            to: "showGlyphs"
            ParallelAnimation {
                PropertyAnimation { target: glyph1; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph2; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph3; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph4; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph5; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph6; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph7; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph8; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph9; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph10; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph11; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph12; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph13; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph14; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph15; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph16; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph17; properties: "width"; duration: 300;  to: 70 }
                PropertyAnimation { target: glyph18; properties: "width"; duration: 300;  to: 70 }
            }
       }
       ]

    states: [
        State {
            name: "hideGlyphs"
            StateChangeScript {
                name: "reloadGlyphs"
                script: {
                    glyph1.source = files.filePath("images", "sol.png")
                    glyph2.source = files.filePath("images", "bird.png")
                    glyph3.source = files.filePath("images", "tree.png")
                    glyph4.source = files.filePath("images", "hand.png")
                    glyph5.source = files.filePath("images", "tree.png")
                    glyph6.source = files.filePath("images", "sierpinski.png")
                    glyph7.source = files.filePath("images", "sol.png")
                    glyph8.source = files.filePath("images", "tree.png")
                    glyph9.source = files.filePath("images", "sol.png")
                    glyph10.source = files.filePath("images", "triangles.png")
                    glyph11.source = files.filePath("images", "sol.png")
                    glyph12.source = files.filePath("images", "triangles.png")
                    glyph13.source = files.filePath("images", "tree.png")
                    glyph14.source = files.filePath("images", "hand.png")
                    glyph15.source = files.filePath("images", "tree.png")
                    glyph16.source = files.filePath("images", "triangles.png")
                    glyph17.source = files.filePath("images", "tree.png")
                    glyph18.source = files.filePath("images", "baum.png")
                    state = "showGlyphs"
                }
            }
        },
        State { name: "showGlyphs" }
    ]

    Component.onCompleted: { state = "hideGlyphs"; }

    Column {
        spacing: 4
        anchors.centerIn: parent

        Row {
            anchors.horizontalCenter: parent.horizontalCenter

            Item {
                width: 70
                height: 70
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    id: glyph1
                    source: files.filePath("images", "tree.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    id: glyph2
                    source: files.filePath("images", "hand.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    id: glyph3
                    source: files.filePath("images", "tree.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    id: glyph4
                    source: files.filePath("images", "boat.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    id: glyph5
                    source: files.filePath("images", "sol.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    id: glyph6
                    source: files.filePath("images", "baum.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    id: glyph7
                    source: files.filePath("images", "tree.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    id: glyph8
                    source: files.filePath("images", "forest.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    id: glyph9
                    source: files.filePath("images", "sol.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    anchors.horizontalCenter: parent.horizontalCenter
                    id: glyph10
                    source: files.filePath("images", "tree.png")
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter

            Item {
                width: 70
                height: 70
                Image {
                    id: glyph11
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: files.filePath("images", "eyes.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    id: glyph12
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: files.filePath("images", "tree.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    id: glyph13
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: files.filePath("images", "sol.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    id: glyph14
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: files.filePath("images", "bird.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    id: glyph15
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: files.filePath("images", "tree.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    id: glyph16
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: files.filePath("images", "boat.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    id: glyph17
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: files.filePath("images", "sol.png")
                }
            }

            Item {
                width: 70
                height: 70
                Image {
                    id: glyph18
                    anchors.horizontalCenter: parent.horizontalCenter
                    source: files.filePath("images", "tree.png")
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 30
            property int fontSize: 24
            property int letterWidth: 48

            function checkText(l1, l2)
            {
                if (l1.text.length > 1) {
                    l1.text = l1.text.substr(l1.text.length - 1, 1)
                }

                if (l1.text.length == 1) {
                    l1.text = l1.text.toUpperCase()
                    l2.forceActiveFocus()
                }
            }

            PlasmaComponents.TextField {
                id: l1
                width: parent.letterWidth
                height: parent.letterWidth
                font.pointSize: parent.fontSize
                onTextChanged: {
                    parent.checkText(l1, l2)
                }
            }

            PlasmaComponents.TextField {
                id: l2
                width: parent.letterWidth
                height: parent.letterWidth
                font.pointSize: parent.fontSize
                onTextChanged: {
                    parent.checkText(l2, l3)
                }
            }

            PlasmaComponents.TextField {
                id: l3
                width: parent.letterWidth
                height: parent.letterWidth
                font.pointSize: parent.fontSize
                onTextChanged: {
                    parent.checkText(l3, l4)
                }
            }

            PlasmaComponents.TextField {
                id: l4
                width: parent.letterWidth
                height: parent.letterWidth
                font.pointSize: parent.fontSize
                onTextChanged: {
                    parent.checkText(l4, l5)
                }
            }

            PlasmaComponents.TextField {
                id: l5
                width: parent.letterWidth
                height: parent.letterWidth
                font.pointSize: parent.fontSize
                onTextChanged: {
                    parent.checkText(l5, l6)
                }
            }

            PlasmaComponents.TextField {
                id: l6
                width: parent.letterWidth
                height: parent.letterWidth
                font.pointSize: parent.fontSize
                onTextChanged: {
                    parent.checkText(l6, l7)
                }
            }

            PlasmaComponents.TextField {
                id: l7
                width: parent.letterWidth
                height: parent.letterWidth
                font.pointSize: parent.fontSize
                onTextChanged: {
                    parent.checkText(l7, l7)
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter

            PlasmaComponents.Button {
                id: submit
                text: i18n("Submit Code")

                onClicked: {
                    enabled = false
                    var doc = new XMLHttpRequest();
                    doc.onreadystatechange = function() {
                        if (doc.readyState == XMLHttpRequest.DONE) {
                            var result = eval('(' + doc.responseText + ')');
                            if (!result.egg  || result.egg.length == 0) {
                                resultDisplay.text = i18n("Incorrect code. Try again.");
                            } else {
                                resultDisplay.text = i18n("Search for '%1'", result.egg);
                            }

                            resultDisplay.visible = true;
                            showBusy.running = false;
                            busyIndicator.visible = false;
                            enabled = true;
                        }
                    }

                    showBusy.running = true;
                    resultDisplay.text = '';
                    var text = l1.text + l2.text + l3.text + l4.text + l5.text + l6.text + l7.text;
                    doc.open("GET", "http://addons.makeplaylive.com:3000/bodega/v1/json/hunt?device=VIVALDI-1&code=" + text);
                    doc.send();
                }

                Timer {
                    id: showBusy
                    interval: 200
                    onTriggered: {
                        busyIndicator.visible = true
                    }
                }
            }
        }

        // vertical spacer
        Item { height: 8; width: 1; }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            Item {
                // this item prevents the layout from jumping about
                height: busyIndicator.height
                width: 1
            }

            PlasmaComponents.BusyIndicator {
                    id: busyIndicator
                    height: 64
                    width: 64
                    visible: false
                    running: visible
            }

            Text {
                height: 64
                id: resultDisplay
                visible: true
            }
        }
    }
}
