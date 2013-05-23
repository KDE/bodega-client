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
import "../components"

BrowserListView {
    id: root
    clip: true

    property string channelId

    abstractItemModel: bodegaClient.session.installJobsModel
    customDelegate: Component {
        id: delegateComponent
        PlasmaComponents.ListItem {
            id: listItem
            enabled: true
            checked: view.currentIndex == index
            Row {
                 id: delegateRow
                 spacing: theme.defaultFont.mSize.width
                    Image {
                        id: iconImage
                        source: model.ImageMediumRole
                        width: theme.mediumIconSize
                        height: width
                    }
                    Column {
                        PlasmaComponents.Label {
                            text: model.DisplayRole
                            width: delegateRow.width - iconImage.width - theme.defaultFont.mSize.width
                            elide: Text.ElideRight
                        }
                        PlasmaComponents.ProgressBar {
                            id: downloadProgress
                            visible: value < maximumValue
                            minimumValue: 0
                            maximumValue: 100
                            value: model.ProgressRole*100
                            height: doneLabel.height
                            anchors {
                                left: parent.left
                                right: parent.right
                            }
                        }
                        PlasmaComponents.Label {
                            id: doneLabel
                            text: i18n("Download finished")
                            height: paintedHeight
                            visible: !downloadProgress.visible
                            font.pointSize: theme.smallestFont.pointSize

                        }
                    }
                }
                onClicked: {
                    console.log(typeof visualDataModel)
                    if (view.currentIndex == index) {
                        return
                    }
                    view.currentIndex = index
                    itemBrowser.pop(browserColumn)

                    if (model.AssetIdRole) {
                        var assetPage = itemBrowser.push(Qt.createComponent("AssetColumn.qml"))
                        assetPage.assetId = model.AssetIdRole
                    }
                }
        }
    }
}
