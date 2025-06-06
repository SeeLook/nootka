/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12

TpopupDialog {
    id: rmLevel

    property string levelName
    property alias levelFile: fileText.text

    signal remove(var fromDisk)

    bgColor: Qt.tint(activPal.window, NOO.alpha("red", 20))
    visible: true
    modal: true
    width: innerCol.width * 1.2
    height: innerCol.height + NOO.factor() * 5
    glowRect.radius: NOO.factor()
    acceptButton.text: NOO.TR("QFileDialog", "Remove")
    acceptButton.pixmap: NOO.pix("delete")
    onAccepted: rmLevel.remove(rmFileChB.checked)

    border {
        color: "red"
        width: NOO.factor() / 4
    }

    Column {
        id: innerCol

        spacing: NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            color: activPal.text
            font.pixelSize: NOO.factor() * 1.5
            text: qsTr("Remove level %1 from the list").arg("<b>" + levelName + "</b>") // + "<br>"
        }

        TcheckBox {
            id: rmFileChB

            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Also delete level file:")
            checked: false
        }

        Text {
            id: fileText

            anchors.horizontalCenter: parent.horizontalCenter
            color: activPal.text
            font.bold: true
        }

    }

}
