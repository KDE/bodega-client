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


/*This is a simple page with the logo on top and a small form in the middle*/

PlasmaComponents.Page {
    id: root

    property bool titleShown: true
    property alias title: titleLabel.text
    default property alias content: contentRect.data

    Image {
        id: logo
        width: 556
        height: 50
        visible: titleShown
        source: viewerPackage.filePath("images", "makeplaylive.png")
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: theme.defaultFont.mSize.height
        }
    }

    PlasmaComponents.Label {
        id: titleLabel
        text: i18n("apps, books, art and more for your device")
        visible: titleShown
        anchors {
            top: logo.bottom
            horizontalCenter: parent.horizontalCenter
        }
    }

    Item {
        id: contentRect
        anchors {
            top: titleShown ? titleLabel.bottom : parent.top
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
}
