/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12

TnotesBarItem {
    id: nBar

    expectedNote: score.note
    z: score.z
    y: score.y + score.height
    width: score.width
    height: nootkaWindow.height / 20
    Component.onCompleted: setAmbitus(GLOB.loNote(), GLOB.hiNote())

    // background
    Rectangle {
        z: -1
        anchors.fill: parent
        color: NOO.alpha(activPal.window, 240)
    }

    GlowRect {
        color: Qt.tint(activPal.base, NOO.alpha((notesDiff === 0 ? GLOB.correctColor : GLOB.wrongColor), 50))
        x: expectedX
        y: (parent.height - height) / 2
        width: nBar.height - NOO.factor() / 2
        height: width

        Text {
            y: -height * 0.2
            anchors.horizontalCenter: parent.horizontalCenter
            color: activPal.text
            text: expectedName

            font {
                family: "Scorek"
                pixelSize: nBar.height / 2
            }

        }

    }

    GlowRect {
        visible: detectedX > 0
        color: activPal.base
        x: detectedX
        y: (parent.height - height) / 2
        width: nBar.height - NOO.factor() / 2
        height: width
        scale: isPlaying ? 1.5 : 1

        Text {
            y: -height * 0.2
            anchors.horizontalCenter: parent.horizontalCenter
            color: activPal.text
            text: detectedName

            font {
                family: "Scorek"
                pixelSize: nBar.height / 2
            }

        }

        Behavior on scale {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 150
            }

        }

        Behavior on x {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 150
            }

        }

    }

    Connections {
        target: GLOB
        onGuitarParamsChanged: setAmbitus(GLOB.loNote(), GLOB.hiNote())
    }

}
