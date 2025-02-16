/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12

TipRect {
    id: statusTip

    property alias text: text.text
    property int tipPos: Item.Top

    x: 0.1 * nootkaWindow.width
    y: statusTip.text === "" ? -1.2 * height : 0
    z: 500
    radius: 0
    width: nootkaWindow.width * 0.8
    height: Math.min(nootkaWindow.height / 6, NOO.factor() * 7)
    color: Qt.tint(ma.pressed ? activPal.highlight : activPal.text, NOO.alpha(NOO.messageColor, 100))

    Connections {
        target: NOO
        onStatusTip: {
            text.textFormat = richText ? Text.RichText : Text.AutoText;
            text.text = statusText;
            statusTip.tipPos = tipPos;
        }
    }

    Text {
        id: text

        padding: statusTip.height / 10
        font.pixelSize: parent.height / 4
        anchors.fill: parent
        wrapMode: Text.WordWrap
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: ma.pressed ? activPal.highlightedText : activPal.base
    }

    MouseArea {
        id: ma

        anchors.fill: parent
        onReleased: NOO.setStatusTip("")
    }

    Behavior on y {
        enabled: GLOB.useAnimations

        SequentialAnimation {
            PauseAnimation {
                duration: 150
            }

            NumberAnimation {
                easing.type: Easing.OutQuad
                duration: 150
            }

        }

    }

}
