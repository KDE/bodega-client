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
import org.kde.plasma.extras 0.1 as PlasmaExtraComponents
import org.kde.qtextracomponents 0.1
import com.coherenttheory.addonsapp 1.0
import "../components"


PlasmaComponents.Page {
    id: root

    property variant paymentMethodJob
    property variant buyJob
    property int amount: 0

    Grid {
        id: mainGrid
        rows: 7
        columns: 2
        spacing: 4
        anchors.centerIn: parent

        PlasmaExtraComponents.Heading {
            text: i18n("Redeem Points Code")
            //HACK: fake mini width to not mess with the Grid layout
            wrapMode: Text.NoWrap
            width: 1
            level: 4
        }
        Item {width: 1; height:1}

        PlasmaComponents.Label {
            text: i18n("Code:")
            anchors {
                right: pointsCode.left
                rightMargin: mainGrid.spacing
            }
        }

        PlasmaComponents.TextField {
            id: pointsCode
            onTextChanged: { redemptionButton.enabled = text.length > 0; }
        }

        Item {width: 1; height:1}
        PlasmaComponents.Button {
            id: redemptionButton
            text: i18n("Redeem code")
            enabled: false
            property variant job

            anchors {
                left: pointsCode.left
            }

            onClicked: {
                redeemBusy.running = true;
                redeemBusy.visible = true;
                enabled = false;

                job = bodegaClient.session.redeemPointsCode(pointsCode.text);
                job.jobFinished.connect(redeemed)
                job.jobError.connect(redeemFailed)
            }

            function redeemed(job)
            {
                if (job.failed) {
                    showMessage(job.error.title, job.error.description, redemptionButton);
                } else {
                    pointsCode.text = '';
                }

                redeemBusy.running = false;
                redeemBusy.visible = false;
                enabled = false;
            }

            function redeemFailed(job, error)
            {
                showMessage(error.title, error.description, redemptionButton)
            }

            PlasmaComponents.BusyIndicator {
                id: redeemBusy;
                visible: false;
                height: redemptionButton.height
                width: height
                anchors {
                    left: parent.right
                }
            }
        }


        //HACK: just for spacing
        Item {width: 1; height: theme.defaultFont.mSize.height}Item {width: 1; height:1}
        PlasmaExtraComponents.Heading {
            text: i18n("Purchase Points")
            level: 4
            //HACK: fake mini width to not mess with the Grid layout
            wrapMode: Text.NoWrap
            width: 1
        }
        Item {width: 1; height:1}

        Item {width: 1; height:1}
        PlasmaComponents.ButtonColumn {
            id: buttonColumn
            exclusive: true
            spacing: mainGrid.spacing

            PlasmaComponents.RadioButton {
                text: i18n("500 points")
                onCheckedChanged: {
                    if (checked) {
                        root.amount = 500
                    }
                }
            }
            PlasmaComponents.RadioButton {
                text: i18n("1000 points")
                onCheckedChanged: {
                    if (checked) {
                        root.amount = 1000
                    }
                }
            }
            PlasmaComponents.RadioButton {
                text: i18n("5000 points")
                onCheckedChanged: {
                    if (checked) {
                        root.amount = 5000
                    }
                }
            }
            PlasmaComponents.RadioButton {
                text: i18n("10,000 points")
                onCheckedChanged: {
                    if (checked) {
                        root.amount = 10000
                    }
                }
            }
            PlasmaComponents.RadioButton {
                text: i18n("Other:")
                onCheckedChanged: {
                    if (checked) {
                        otherField.forceActiveFocus()
                        root.amount = otherField.text
                    }
                }
                PlasmaComponents.TextField {
                    id: otherField
                    anchors {
                        left: parent.right
                        leftMargin: mainGrid.spacing * 2
                    }
                    onTextChanged: {
                        root.amount = otherField.text
                    }
                }
            }
        }

        Item {width: 1; height:1}
        PlasmaComponents.Button {
            id: buyButton
            enabled: root.amount > 0
            text: i18n("Purchase")
            onClicked: {
                paymentMethodJob = bodegaClient.session.paymentMethod()
                paymentMethodJob.jobFinished.connect(paymentMethodJobFinished)
            }
        }
    }

    function paymentMethodJobFinished()
    {
        if (paymentMethodJob.failed) {
            showMessage(paymentMethodJob.error.title, paymentMethodJob.error.id + ": " + paymentMethodJob.error.description);
        }
        
        var cardData = paymentMethodJob.parsedJson
        //check if the paymentMethod exists *and* there is data
        //FIXME: more reliable validity chaching that the last4?
        //there should be a back and forth to payment method page until the actual payment succeeded
        if (!paymentMethodJob.failed) {
            //Do the actual payment
            buyJob = bodegaClient.session.buyPoints(root.amount)
            buyJob.jobFinished.connect(buyJobFinished)
        } else {
            root.pageStack.push(Qt.createComponent("PaymentMethodStack.qml"))
        }
    }

    function buyJobFinished()
    {
        if (buyJob.failed) {
            showMessage(buyJob.error.title, buyJob.error.errorId + ": " + buyJob.error.description, buyButton);

            print("Error")
            for (var i in buyJob.error) {
                print(i + ": " + buyJob.error[i])
            }
        } else {
            print("Answer")
            var response = buyJob.parsedJson
            for (var i in response) {
                print(i + ": " + response[i])
            }
            if (root.pageStack.depth == 1) {
                showMessage("", i18n("%1 points successfully purchased", root.amount), buyButton)
            } else {
                root.pageStack.pop()
            }
        }
    }
}
