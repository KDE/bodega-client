/*
 *   Copyright 2013 Giorgos Tsiapaliokas <terietor@gmail.com>
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
import "../components"

BrowserListView {
    id: root
    property variant rootIndex
    abstractItemModel: bodegaClient.assetRatingsModel

    customHeader: PlasmaComponents.Label {
        text: i18n("All ratings")
        anchors.horizontalCenter: parent.horizontalCenter
    }

    customDelegate: Component {
        id: assetRatingsModelDelegate
        PlasmaComponents.ListItem {
            id: listItem
            property variant ratings: model.Ratings
            Column {
                width: parent.width

                Row {
                    x: Math.max(0, parent.width / 2 - theme.defaultFont.mSize.width * 11)
                    spacing: 4
                    PlasmaComponents.Label {
                        text: i18n("Rated by:", model.PersonName)
                        wrapMode: Text.WordWrap
                        width: theme.defaultFont.mSize.width*11
                        horizontalAlignment: Text.AlignRight
                    }

                    PlasmaComponents.Label {
                        text: model.PersonName
                    }
                }

                Row {
                    x: Math.max(0, parent.width / 2 - theme.defaultFont.mSize.width * 11)
                    spacing: 4
                    PlasmaComponents.Label {
                        text: i18n("On:", model.PersonName)
                        wrapMode: Text.WordWrap
                        width: theme.defaultFont.mSize.width*11
                        horizontalAlignment: Text.AlignRight
                    }

                    PlasmaComponents.Label {
                        text: Qt.formatDateTime(model.Rated)
                    }
                }

                Repeater {
                    model: ratings.length
                    delegate: Row {
                        x: Math.max(0, parent.width / 2 - theme.defaultFont.mSize.width * 11)
                        spacing: 4
                        PlasmaComponents.Label {
                            text: i18n("%1:", ratings[index]["AttributeName"])
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
            }
        }
    }
}

