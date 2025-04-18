/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

CheckBox {
    id: chB

    property alias textColor: textItem.color
    property alias textItem: textItem

    font.pixelSize: NOO.factor()

    indicator: TipRect {
        implicitWidth: NOO.factor() * 1.75
        implicitHeight: NOO.factor() * 1.75
        x: chB.leftPadding
        y: (chB.height - height) / 2
        horizontalOffset: rised ? NOO.factor() / 8 : 0
        verticalOffset: horizontalOffset
        rised: !chB.down
        color: chB.enabled ? activPal.base : Qt.darker(disdPal.window, 1.2)
        scale: chB.pressed ? 0.9 : 1

        Rectangle {
            width: parent.width
            height: parent.height / 7
            anchors.centerIn: parent
            color: chB.enabled ? activPal.dimText : disdPal.text
            rotation: 45
            radius: height / 2
            visible: chB.checked
        }

        Rectangle {
            width: parent.width
            height: parent.height / 7
            anchors.centerIn: parent
            color: chB.enabled ? activPal.dimText : disdPal.text
            rotation: 135
            radius: height / 2
            visible: chB.checked
        }

        Behavior on scale {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 150
            }

        }

    }

    contentItem: Text {
        id: textItem

        y: (chB.height - height) / 2
        text: chB.text
        topPadding: chB.font.pixelSize * 0.4
        leftPadding: indicator.width + chB.font.pixelSize / 2
        color: chB.enabled ? activPal.text : disdPal.text
        style: chB.activeFocus ? Text.Sunken : Text.Normal
        styleColor: activPal.highlight

        font {
            pixelSize: chB.font.pixelSize
            family: chB.font.family
        }

    }

}
