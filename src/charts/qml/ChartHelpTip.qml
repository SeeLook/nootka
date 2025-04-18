/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12

TipRect {
    id: helpTip

    property alias text: txt.text
    property color hintColor: GLOB.correctColor

    shadowRadius: NOO.factor()
    width: txt.width + 2 * NOO.factor()
    height: txt.height + 2 * NOO.factor()
    z: 150
    color: Qt.tint(activPal.base, NOO.alpha(hintColor, 50))

    border {
        color: hintColor
        width: 1
    }

    anchors {
        centerIn: parent
    }

    Column {
        padding: NOO.factor()

        Text {
            id: txt

            z: 200
            color: activPal.text
            textFormat: Text.RichText
            horizontalAlignment: Text.AlignHCenter
            onLinkActivated: analyzeWindow.openExam()

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.NoButton
                cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
            }

        }

    }

}
