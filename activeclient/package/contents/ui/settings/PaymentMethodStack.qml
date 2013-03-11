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
import "../components"


PlasmaComponents.PageStack {
    id: paymentMethodStack

    property variant job

    property string name
    property string cardType
    property string last4
    property string country
    property string address1
    property string address2
    property string state
    property string zip

    PlasmaComponents.BusyIndicator {
        id: busyIndicator
        visible: false
        anchors.centerIn: parent
    }

    function loadData()
    {
        busyIndicator.visible = true;
        busyIndicator.running = true;
        job = bodegaClient.session.paymentMethod();
        job.jobFinished.connect(jobFinished);
    }

    function jobFinished()
    {
        if (job.failed) {
            paymentMethodStack.push(Qt.createComponent("PaymentMethodEdit.qml"))
        } else {
            var card = job.parsedJson.card;
            if (card.type) cardType = card.type;
            if (card.last4) last4 = card.last4;
            if (card.address_country) country = card.address_country;
            if (card.address_line1) address1 = card.address_line1;
            if (card.address_line2) address2 = card.address_line2;
            if (card.address_state) state = card.address_state;
            if (card.address_zip) zip = card.address_zip;
            paymentMethodStack.push(Qt.createComponent("PaymentMethodView.qml"))
        }

        busyIndicator.running = false;
        busyIndicator.visible = false;
    }

    Component.onCompleted: loadData()
}
