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
import com.makeplaylive.addonsapp 1.0
import "../components"

BrowserColumn {
    id: root

    anchors.leftMargin: 3

    property variant assetOperations
    property int assetId: 0
    property QtObject session: bodegaClient.session

    property variant installJob: null
    property int installStatus: 0

    function downloadAsset() {downloadProgress.opacity = 1
        bodegaClient.installJobScheduler.scheduleInstall(assetId, session)
    }

    function reloadPage() {
        if (assetId > 0) {
            assetOperations = root.session.assetOperations(assetId);

            root.installJob = bodegaClient.installJobScheduler.installJobForAsset(assetId)
            if (root.installJob) {
                downloadProgress.opacity = 1
                root.installJob.progressChanged.connect(downloadProgress.updateProgress)
                root.installJob.jobFinished.connect(downloadProgress.operationFinished)
                root.installJob.jobError.connect(downloadProgress.installError)
                root.installJob.jobFinished.connect(installButton.assetOpJobCompleted)
            }
        }
    }

    onAssetIdChanged: reloadPage()

    Connections {
        target: bodegaClient.installJobScheduler
        onInstallStatusChanged: {
            if (assetId == root.assetId) {
                root.installStatus = status;
                if (status == InstallJobScheduler.Installing) {
                    downloadProgress.opacity = 1
                    downloadProgress.indeterminate = true
                    downloadProgress.indeterminate = false
                    root.installJob = bodegaClient.installJobScheduler.installJobForAsset(assetId);

                    if (root.installJob) {
                        root.installJob.progressChanged.connect(downloadProgress.updateProgress)
                        root.installJob.jobFinished.connect(downloadProgress.operationFinished)
                        root.installJob.jobError.connect(downloadProgress.installError)
                        root.installJob.jobFinished.connect(installButton.assetOpJobCompleted)
                    }
                }
            }
        }
    }

    PlasmaExtras.ScrollArea {
        id: scrollArea
        anchors.fill: parent
        //color: theme.backgroundColor

        Flickable {
            id: mainFlickable
            anchors.fill: parent
            flickableDirection: Flickable.VerticalFlick

            contentWidth: width
            contentHeight: mainColumn.height

            Item {
                width: mainFlickable.width
                height: mainColumn.height
                Baloon {
                    id: questionBaloon
                    visualParent: installButton
                    property bool canPurchase:(root.session.points >= assetOperations.assetInfo.points)
                    Column {
                        spacing: 4
                        width: theme.defaultFont.mSize.width*18
                        PlasmaComponents.Label {
                            anchors {
                                left: parent.left
                                right:parent.right
                            }
                            text: {
                                if (questionBaloon.canPurchase) {
                                    i18n("Confirm purchase of \"%1\" for %2 points.", assetOperations.assetInfo.name,  assetOperations.assetInfo.points)
                                } else {
                                    i18n("You need to buy %1 extra points to be able to buy this asset.",  assetOperations.assetInfo.points)
                                }
                            }
                            wrapMode: Text.Wrap
                        }
                        PlasmaComponents.Button {
                            text: questionBaloon.canPurchase ? i18n("Confirm") : i18n("Add Points")
                            anchors.horizontalCenter: parent.horizontalCenter
                            onClicked: {
                                if (questionBaloon.canPurchase) {
                                    // purchase
                                    downloadProgress.opacity = 1
                                    downloadProgress.indeterminate = true
                                    var job = root.session.purchaseAsset(assetId)
                                    job.jobFinished.connect(downloadProgress.operationFinished)
                                    job.jobFinished.connect(root.downloadAsset)
                                    job.jobFinished.connect(installButton.assetOpJobCompleted)
                                    job.jobError.connect(downloadProgress.installError)
                                    root.installJob = job
                                } else {
                                    mainStack.push(Qt.createComponent("../settings/AddPoints.qml"))
                                }
                                questionBaloon.close()
                            }
                        }
                    }
                }
                InlineConfirmationDialog {
                    id: uninstallConfirmation
                    visualParent: installButton
                    message: i18n("Are you sure you want to uninstall %1?", assetOperations.assetInfo.name)
                    onAccepted: {
                        downloadProgress.opacity = 1
                        downloadProgress.indeterminate = true
                        var job = root.session.uninstall(assetOperations)
                        job.jobFinished.connect(downloadProgress.operationFinished)
                        job.error.connect(downloadProgress.installError)
                        job.jobFinished.connect(installButton.assetOpJobCompleted)
                        if (job.finished) {
                            downloadProgress.opacity = 0
                            enabled = true;
                        }
                    }
                }
                Column {
                    id: mainColumn
                    width: mainFlickable.width
                    anchors {
                        left: parent.left
                        right: parent.right
                        leftMargin: theme.defaultFont.mSize.width
                        rightMargin: theme.defaultFont.mSize.width
                    }

                    spacing: theme.defaultFont.mSize.height

                    Item {
                        anchors {
                            left: parent.left
                            right: parent.right
                            margins: 32
                        }
                        height: bigIconImage.visible ? width : theme.defaultFont.mSize.height
                        Image {
                            id: bigIconImage
                            source: assetOperations.assetInfo.images["huge"]
                            asynchronous: true
                            visible: bigIconImage.status == Image.Ready

                            anchors.centerIn: parent
                            fillMode: Image.PreserveAspectFit
                            //width:  Math.min(256 * ratio, Math.min(parent.width - 32, sourceSize.width))
                            width:  Math.min(parent.width, sourceSize.width)
                            height: Math.min(parent.height, sourceSize.height)

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
                    }

                    PlasmaExtras.Heading {
                        id: titleLabel
                        level: 1
                        text: assetOperations.assetInfo.name
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                    }
                    PlasmaComponents.Label {
                        text: assetOperations.assetInfo.points > 0 ? i18nc("Price in points", "%1 points", assetOperations.assetInfo.points) +
                                                                     (root.assetOperations.assetInfo.canDownload ? "\n" + i18n("Purchased") : '')
                                                                   : i18n("Free")
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
                                indeterminate = (value >= 100)
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
                                showMessage(error.title, error.description, installButton)
                            }
                            Behavior on opacity {
                                NumberAnimation {
                                    duration: 250
                                    easing.type: Easing.InOutQuad
                                }
                            }
                        }
                    }
                    ExpandingLabel {
                        id: descriptionLabel
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                        visible: text != ''
                        text: assetOperations.assetInfo.description
                    }

                    PlasmaComponents.Button {
                        visible: assetOperations.installed && assetOperations.launchText !== ""
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: assetOperations.launchText
                        onClicked: assetOperations.launch()
                    }

                    PlasmaComponents.Button {
                        id: installButton
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: root.installJob == null && root.assetOperations.ready
                        visible: !root.assetOperations.installed || bodegaClient.updatedAssetsModel.containsAsset(assetId)
                        text: {
                            if (bodegaClient.updatedAssetsModel.containsAsset(assetId)) {
                                i18n("Update")
                            } else {
                                root.assetOperations.assetInfo.canDownload ? i18n("Download") : i18n("Purchase")
                            }
                        }
                        onClicked: {
                            if (root.assetOperations.assetInfo.canDownload) {
                                root.downloadAsset()
                            } else {
                                //ask to purchase
                                questionBaloon.open()
                            }
                        }

                        function assetOpJobCompleted()
                        {
                            //TODO: need to show a success message methinks!
                            root.assetOperations = root.session.assetOperations(assetId)
                            root.installJob = null
                        }
                    }

                    PlasmaComponents.Button {
                        id: uninstallButton
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: root.installJob == null && root.assetOperations.ready
                        visible: root.assetOperations.installed
                        text: i18n("Remove")
                        onClicked: uninstallConfirmation.open()
                    }

                    PlasmaCore.SvgItem {
                        anchors {
                            left: parent.left
                            right: parent.right
                            leftMargin: -theme.defaultFont.mSize.width
                            rightMargin: -theme.defaultFont.mSize.width
                        }
                        svg: PlasmaCore.Svg {
                            imagePath: "widgets/line"
                        }
                        elementId: "horizontal-line"
                    }

                    RatingWidget {
                        id: ratingWidget
                        anchors {
                            left: parent.left
                            right: parent.right
                        }
                    }

                    PlasmaCore.SvgItem {
                        anchors {
                            left: parent.left
                            right: parent.right
                            leftMargin: -theme.defaultFont.mSize.width
                            rightMargin: -theme.defaultFont.mSize.width
                        }
                        svg: PlasmaCore.Svg {
                            imagePath: "widgets/line"
                        }
                        elementId: "horizontal-line"
                    }

                    Grid {
                        anchors.horizontalCenter: parent.horizontalCenter
                        rows: 5
                        columns: 2
                        PlasmaComponents.Label {
                            id: authorTitle
                            verticalAlignment: Text.AlignTop
                            anchors {
                                top: authorLabel.top
                                right: authorLabel.left
                                rightMargin: theme.defaultFont.mSize.width
                            }
                            text: i18n("Author:")
                            visible: authorLabel.visible
                        }
                        PlasmaComponents.Label {
                            id: authorLabel
                            verticalAlignment: Text.AlignTop
                            visible: assetOperations.assetTags && assetOperations.assetTags.author != undefined && assetOperations.assetTags.author[0] != ""
                            text: visible ? assetOperations.assetTags.author[0] : ''
                            width: root.width - authorTitle.width - 40
                            wrapMode: Text.WordWrap
                        }
                        PlasmaComponents.Label {
                            verticalAlignment: Text.AlignTop
                            visible: sizeLabel.visible
                            anchors {
                                right: sizeLabel.left
                                rightMargin: theme.defaultFont.mSize.width
                            }
                            text: i18n("Size:")
                        }
                        PlasmaComponents.Label {
                            verticalAlignment: Text.AlignTop
                            id: sizeLabel
                            visible: text != ''
                            text: assetOperations.assetInfo.formatedSize
                            wrapMode: Text.WordWrap
                        }
                        PlasmaComponents.Label {
                            verticalAlignment: Text.AlignTop
                            anchors {
                                right: dateLabel.left
                                rightMargin: theme.defaultFont.mSize.width
                            }
                            text: i18n("Date:")
                            visible: dateLabel.visible
                        }
                        PlasmaComponents.Label {
                            verticalAlignment: Text.AlignTop
                            visible: versionLabel.visible
                            anchors {
                                right: versionLabel.left
                                rightMargin: theme.defaultFont.mSize.width
                            }
                            text: i18n("Version:")
                        }
                        PlasmaComponents.Label {
                            verticalAlignment: Text.AlignTop
                            id: versionLabel
                            visible: text != ''
                            text: AssetVersionRole
                            wrapMode: Text.WordWrap
                        }
                        PlasmaComponents.Label {
                            verticalAlignment: Text.AlignTop
                            anchors {
                                right: dateLabel.left
                                rightMargin: theme.defaultFont.mSize.width
                            }
                            text: i18n("Date:")
                            visible: dateLabel.visible
                        }
                        PlasmaComponents.Label {
                            id: dateLabel
                            verticalAlignment: Text.AlignTop
                            visible: assetOperations.assetTags && assetOperations.assetTags.created != undefined && assetOperations.assetTags.created[0] != ""
                            property variant splitDate: visible ? assetOperations.assetTags.created[0].split("-") : Date()
                            text: Qt.formatDate(new Date(splitDate[0], splitDate[1], splitDate[2]), Qt.DefaultLocaleShortDate)
                        }
                        PlasmaComponents.Label {
                            id: licenseTitle
                            verticalAlignment: Text.AlignTop
                            anchors {
                                right: licenseLabel.left
                                rightMargin: theme.defaultFont.mSize.width
                            }
                            text: i18n("License:")
                            visible: licenseLabel.visible
                        }
                        PlasmaComponents.Label {
                            id: licenseLabel
                            verticalAlignment: Text.AlignTop
                            visible: text != ''
                            text: assetOperations.assetInfo.assetLicenseText != '' ? '<a href="' + assetOperations.assetInfo.assetLicenseText + '">' + assetOperations.assetInfo.license + '</a>'
                                                                                : assetOperations.assetInfo.license
                            width: root.width - licenseTitle.width - 40
                            wrapMode: Text.WordWrap
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
        }
    }
}
