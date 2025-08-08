/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick.Effects
import QtQuick 2.12

Item {
    id: resultTip

    property real targetY: shortEdge / 80
    property alias color: txt.color
    property alias text: txt.text
    property real shortEdge: Math.min(executor.height, executor.width)

    width: txt.width
    height: txt.height
    z: 60000 // above status tip rectangle
    transformOrigin: Item.Top
    Component.onCompleted: {
        if (GLOB.useAnimations)
            anim.running = true;

        y = GLOB.useAnimations ? -2 * height : targetY;
    }

    anchors {
        right: parent.right
        rightMargin: shortEdge / 12
    }

    Text {
        id: txt

        visible: false
        horizontalAlignment: Text.AlignHCenter
        textFormat: Text.StyledText

        font {
            pixelSize: (shortEdge / 15) * (1 - 0.2 * (lineCount - 1))
            bold: true
        }

    }

    MultiEffect {
        anchors.fill: txt
        source: txt
        shadowEnabled: true
        shadowHorizontalOffset: txt.font.pixelSize / 12
        shadowVerticalOffset: txt.font.pixelSize / 12
        shadowColor: activPal.shadow
    }

    SequentialAnimation {
        id: anim

        NumberAnimation {
            target: resultTip
            property: "y"
            to: targetY
            duration: 200
        }

        NumberAnimation {
            target: resultTip
            property: "scale"
            to: 2
            duration: 200
        }

        NumberAnimation {
            target: resultTip
            property: "scale"
            to: 1
            duration: 300
        }

    }

}
