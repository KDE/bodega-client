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
import "../components"

BrowserColumn {
    id: root

    property variant assetOperations
    property int assetId: 0

    property variant installJob: null

    onAssetIdChanged: {
        if (assetId > 0) {
            assetOperations = bodegaClient.session.assetOperations(assetId);
            
            root.installJob = bodegaClient.session.installJobsModel.jobForAsset(root.assetId)
            if (root.installJob) {
                downloadProgress.opacity = 1
                root.installJob.progressChanged.connect(downloadProgress.updateProgress)
                root.installJob.jobFinished.connect(downloadProgress.operationFinished)
                root.installJob.jobError.connect(downloadProgress.installError)
                root.installJob.jobFinished.connect(installButton.assetOpJobCompleted)
            }
        }
    }

    Item {
        anchors.fill: parent
        //color: theme.backgroundColor

        Flickable {
            id: mainFlickable
            anchors {
                fill: parent
                margins: 8
            }
            contentWidth: width
            contentHeight: mainColumn.height

            Column {
                id: mainColumn
                width: mainFlickable.width

                spacing: 8
                Image {
                    id: bigIconImage
                    source: assetOperations.assetInfo.images["huge"]
                    asynchronous: true

                    anchors.horizontalCenter: parent.horizontalCenter
                    width:  Math.min(256 * (sourceSize.width / sourceSize.height), Math.min(parent.width - 32, sourceSize.width))
                    //width:  Math.min(parent.width - 32, sourceSize.width)
                    height: width / (sourceSize.width/sourceSize.height)

                    /*
                    TODO: make this show the large image unscaled
                    MouseArea {
                        anchors.fill: parent
                        property bool big: false
                        onClicked: {
                            if (big) {
                                bigIconImage.source = assetOperations.assetInfo.images["large"];
                            } else {
                                bigIconImage.source = assetOperations.assetInfo.images["huge"];
                            }

                            big = !big;
                        }
                    }
                    */
                }
                PlasmaComponents.Label {
                    id: titleLabel
                    text: assetOperations.assetInfo.name
                    font.pointSize: 20
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                }
                PlasmaComponents.Label {
                    text: assetOperations.assetInfo.points > 0 ? i18nc("Price in points", "%1 points", assetOperations.assetInfo.points) : i18n("Free")
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    height: downloadProgress.height
                    PlasmaComponents.ProgressBar {
                        id: downloadProgress
                        opacity: 0
                        minimumValue: 0
                        maximumValue: 100
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        function updateProgress(progress)
                        {
                            value = progress*100
                        }
                        onValueChanged: {
                            if (value >= 100) {
                                indeterminate = true
                            }
                        }
                        function operationFinished(job)
                        {
                            opacity = 0
                            indeterminate = false
                        }
                        function installError(job, error)
                        {
                            opacity = 0
                            indeterminate = false
                            showMessage(error.title, error.description)
                        }
                        Behavior on opacity {
                            NumberAnimation {
                                duration: 250
                                easing.type: Easing.InOutQuad
                            }
                        }
                    }
                }
                PlasmaComponents.Button {
                    id: installButton
                    anchors.horizontalCenter: parent.horizontalCenter
                    enabled: root.installJob == null && assetOperations.isReady
                    text: {
                        if (assetOperations.installed) {
                            i18n("Uninstall")
                        } else {
                            assetOperations.assetInfo.canDownload ? i18n("Download") : i18n("Purchase")
                        }
                    }
                    onClicked: {
                        downloadProgress.opacity = 1
                        downloadProgress.indeterminate = true

                        if (assetOperations.installed) {
                            var job = bodegaClient.session.uninstall(assetOperations)
                            job.jobFinished.connect(downloadProgress.operationFinished)
                            job.error.connect(downloadProgress.installError)
                            job.jobFinished.connect(assetOpJobCompleted)
                            if (job.finished) {
                                downloadProgress.opacity = 0
                                enabled = true;
                            }
                        } else if (assetOperations.assetInfo.canDownload) {
                           downloadProgress.indeterminate = false
                           root.installJob = bodegaClient.session.install(assetOperations)
                           root.installJob.progressChanged.connect(downloadProgress.updateProgress)
                           root.installJob.jobFinished.connect(downloadProgress.operationFinished)
                           root.installJob.jobError.connect(downloadProgress.installError)
                           root.installJob.jobFinished.connect(assetOpJobCompleted)
                        } else {
                            // purchase
                            var job = bodegaClient.session.purchaseAsset(assetId)
                            job.jobFinished.connect(downloadProgress.operationFinished)
                            job.jobFinished.connect(assetOpJobCompleted)
                            job.jobError.connect(downloadProgress.installError)
                        }
                    }

                    function assetOpJobCompleted()
                    {
                        //TODO: need to show a success message methinks!
                        root.assetOperations = bodegaClient.session.assetOperations(assetId)
                    }
                }
                PlasmaComponents.Button {
                    visible: assetOperations.installed && assetOperations.launchText !== ""
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: assetOperations.launchText
                    onClicked: assetOperations.launch()
                }
                //TODO: make a component out of it
                ExpandingLabel {
                    id: descriptionLabel
                    text: assetOperations.assetInfo.description
                }

                Grid {
                    anchors.horizontalCenter: parent.horizontalCenter
                    rows: 3
                    columns: 2
                    PlasmaComponents.Label {
                        id: authorTitle
                        anchors {
                            right: authorLabel.left
                            rightMargin: theme.defaultFont.mSize.width
                        }
                        text: i18n("Author:")
                        visible: authorLabel.visible
                    }
                    PlasmaComponents.Label {
                        id: authorLabel
                        visible: assetOperations.assetTags && assetOperations.assetTags.author != undefined && assetOperations.assetTags.author[0] != ""
                        text: visible ? assetOperations.assetTags.author[0] : ''
                        width: root.width - authorTitle.width - 40
                        wrapMode: Text.WordWrap
                    }
                    PlasmaComponents.Label {
                        anchors {
                            right: versionLabel.left
                            rightMargin: theme.defaultFont.mSize.width
                        }
                        text: i18n("Version:")
                    }
                    PlasmaComponents.Label {
                        id: versionLabel
                        text: AssetVersionRole
                        wrapMode: Text.WordWrap
                    }
                    PlasmaComponents.Label {
                        anchors {
                            right: dateLabel.left
                            rightMargin: theme.defaultFont.mSize.width
                        }
                        text: i18n("Date:")
                        visible: dateLabel.visible
                    }
                    PlasmaComponents.Label {
                        id: dateLabel
                        visible: assetOperations.assetTags && assetOperations.assetTags.created != undefined && assetOperations.assetTags.created[0] != ""
                        property variant splitDate: visible ? assetOperations.assetTags.created[0].split("-") : Date()
                        text: Qt.formatDate(new Date(splitDate[0], splitDate[1], splitDate[2]), Qt.DefaultLocaleShortDate)
                    }
                }

                /*TODO
                SlideShow {
                    model: ListModel {
                        ListElement {
                            fileName: "../../storebrowser/kpat1.jpg"
                        }
                        ListElement {
                            fileName: "../../storebrowser/kpat2.jpg"
                        }
                    }
                }*/
            }
        }
        PlasmaComponents.ScrollBar {
            orientation: Qt.Vertical
            flickableItem: mainFlickable
        }
    }
}
