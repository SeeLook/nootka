/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

AbstractButton {
    id: rectButt

    property string statusTip: ""
    property int statusPos: Item.TopLeft
    property alias textColor: tx.color
    //* Properties to handle Scorek glyphs which are out of rectangle
    property alias forcedHeight: bg.height
    property alias yOffset: tx.topPadding

    height: parent.height
    hoverEnabled: true
    onHoveredChanged: {
        if (GLOB.showHints && statusTip !== "")
            NOO.setStatusTip(hovered ? statusTip : "", statusPos);

    }
    onClicked: focus = false // keep focus free - do not lock up space key

    contentItem: Text {
        id: tx

        leftPadding: NOO.factor() / 2
        font: rectButt.font
        text: rectButt.text
        color: checked ? "red" : activPal.text
        scale: pressed ? 0.7 : 1

        Behavior on scale {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 150
            }

        }

    }

    background: Rectangle {
        id: bg

        width: contentItem.width + NOO.factor() / 2
        height: parent.height
        color: activPal.highlight
        radius: height / 5
        scale: hovered ? 1 : 0

        Behavior on scale {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 150
            }

        }

    }

}
