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
    abstractItemModel: bodegaClient.updatedAssetsModel

    property variant rootIndex
    property string channelId

    anchors.leftMargin: 3

    VisualDataModel {
        id: channelsModel
        model: bodegaClient.channelsModel
        rootIndex: root.rootIndex
        delegate: delegateComponent
    }

    PlasmaExtras.Heading {
        anchors.centerIn: parent
        width: parent.width * 0.8
        visible: bodegaClient.updatedAssetsModel.count == 0
        text: i18n("Everything is up to date")
        horizontalAlignment: Text.AlignHCenter
    }
    

    PlasmaComponents.ToolBar {
        id: toolBar
        anchors.bottom: parent.bottom
        z: 1
        visible: bodegaClient.updatedAssetsModel.count > 0
        tools: Item {
            height: childrenRect.height
            PlasmaComponents.Button {
                anchors.centerIn: parent
                text: i18n("Update All")
                onClicked: bodegaClient.updatedAssetsModel.updateAll();
            }
        }
    }

    customDelegate: Component {
        id: delegateComponent
        StoreListItem {
            checked: view.currentIndex == index
            onClicked: {
                if (view.currentIndex == index) {
                    return
                }
                view.currentIndex = index
                itemBrowser.pop(root)

                if (model.AssetIdRole) {
                    var assetPage = itemBrowser.push(Qt.createComponent("AssetColumn.qml"))
                    assetPage.assetId = model.AssetIdRole
                    assetPage.session = model.SessionRole
                }
            }
        }
    }
}
