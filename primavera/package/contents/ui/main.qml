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

    Column {
        spacing: 4
        anchors.centerIn: parent

        Row {
            anchors.horizontalCenter: parent.horizontalCenter

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "tree.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "hand.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "tree.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "boat.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "sol.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "baum.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "tree.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "forest.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "tree.png")
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "eyes.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "tree.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "sol.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "bird.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "tree.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "boat.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "sol.png")
            }

            Image {
                width: 70
                height: 70
                source: files.filePath("images", "tree.png")
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
