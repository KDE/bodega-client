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
    abstractItemModel: bodegaClient.collectionAssetsModel

    VisualDataModel {
        id: collectionAssetsModel
        model: bodegaClient.collectionAssetsModel
        rootIndex: root.rootIndex
        delegate: collectionAssetsModelDelegate
    }

    customDelegate: Component {
        id: collectionAssetsModelDelegate
        PlasmaComponents.ListItem {
            id: listItem
            enabled: true
            checked: view.currentIndex == index
            Row {
                /*anchors {
                    left: parent.left
                    right: parent.right
                }*/
                id: delegateRow
                spacing: theme.defaultFont.mSize.width
                PlasmaComponents.Label {
                    text: model.AssetNameRole
                }
            }

            onClicked: {
                if (view.currentIndex == index) {
                    return
                }
                view.currentIndex = index
                itemBrowser.pop(root)
                if (model.AssetIdRole) {
                    var assetPage = itemBrowser.push(Qt.createComponent("AssetColumn.qml"));
                    assetPage.assetId = model.AssetIdRole;
                }
            }
        }
    }
}

