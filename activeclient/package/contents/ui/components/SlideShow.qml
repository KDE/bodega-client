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
import org.kde.qtextracomponents 0.1
import "./private"

PlasmaCore.FrameSvgItem {
    id: photoBackground
    imagePath: "widgets/media-delegate"
    prefix: "picture"
    property alias model: slideShowImplementation.model
    visible: model.count > 0

    anchors {
        left: parent.left
        right: parent.right
        margins: 4
    }
    height: width/1.6

    SlideShowImplementation {
        id: slideShowImplementation
        anchors {
            fill: parent
            leftMargin: photoBackground.margins.left
            topMargin: photoBackground.margins.top
            rightMargin: photoBackground.margins.right
            bottomMargin: photoBackground.margins.bottom
        }
        onClicked: {
            if (dialog == undefined) {
                dialog = dialogComponent.createObject(root)
            }
            dialog.currentIndex = index
            dialog.open()
        }
    }
    property Item dialog
    Component {
        id: dialogComponent
        PlasmaComponents.Dialog {
            id: fullscreenDialog
            property alias currentIndex: dialogSlideshow.currentIndex
            content: SlideShowImplementation {
                id: dialogSlideshow
                model: slideShowImplementation.model
                onClicked: fullscreenDialog.close()
                width: appRoot.width * 0.7
                height: appRoot.height * 0.7
            }
        }
    }
}
