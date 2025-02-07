/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

Tile {
    property alias maxStep: maxSpin.value

    signal maxModified()

    description: qsTr("Maximal interval between notes used in a melody (in semitones).")

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: NOO.factor() / 2

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Max interval")
            color: activPal.text
        }

        TspinBox {
            id: maxSpin

            from: 2
            to: 36
            value: 0
            onValueModified: maxModified()
        }

    }

}
