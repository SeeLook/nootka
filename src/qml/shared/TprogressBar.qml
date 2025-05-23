/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

ProgressBar {
    id: control

    property real tickWidth: Screen.pixelDensity * 0.7
    property real tickGap: tickWidth * 1.2

    hoverEnabled: true

    background: Rectangle {
        width: control.width
        height: control.height * 0.8
        y: control.height * 0.1
        color: activPal.window
    }

    contentItem: Item {
        width: control.width
        height: control.height * 0.8
        y: control.height * 0.1

        Repeater {
            model: control.width / (tickGap + tickWidth)

            Rectangle {
                color: x <= control.position * control.width ? GLOB.correctColor : activPal.base
                width: tickWidth
                radius: tickWidth / 2
                height: parent.height
                x: index * (tickGap + tickWidth)
            }

        }

        Rectangle {
            id: percentRect

            width: percentText.width
            height: control.height
            anchors.centerIn: parent
            color: activPal.window
            opacity: control.hovered ? 1 : 0

            Text {
                id: percentText

                anchors.centerIn: parent
                font.pointSize: percentRect.height * 0.8
                text: " " + Math.round(control.position * 100) + " % "
            }

            Behavior on opacity {
                enabled: GLOB.useAnimations

                PropertyAnimation {
                }

            }

        }

    }

}
