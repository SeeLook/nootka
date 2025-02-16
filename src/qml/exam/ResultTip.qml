/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Qt5Compat.GraphicalEffects
import QtQuick 2.12

Item {
    id: resultTip

    property real targetY: shortEdge / 80
    property alias color: txt.color
    property alias text: txt.text
    property real shortEdge: Math.min(executor.height, executor.width)

    width: txt.width
    height: txt.height
    z: 501 // above status tip rectangle
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

    DropShadow {
        anchors.fill: txt
        horizontalOffset: txt.font.pixelSize / 12
        verticalOffset: horizontalOffset
        radius: shortEdge / 100
        samples: radius * 2 + 1
        color: activPal.shadow
        source: txt
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
