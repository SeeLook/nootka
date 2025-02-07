/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import Score 1.0

TdummyChord {
    id: chordIt

    // private
    property var chV: importWindow.chordView

    anchors.fill: parent

    Rectangle {
        //anchors.fill: parent
        width: parent.width + 2
        height: loPosY - hiPosY + 8
        x: -1
        y: hiPosY - 4
        radius: width / 4
        color: NOO.alpha(chV && chV.visible && chV.chordIt == chordIt ? activPal.highlight : activPal.dimText, ma.containsMouse ? 60 : 30)
    }

    Repeater {
        model: chordModel

        Text {
            scale: index === selected ? 1.4 : 1.2
            text: noteHead
            y: headPos(index) - 15 + (chordIt.parent ? 0 : 0) // force to refresh note-heads position
            color: index === selected ? activPal.text : activPal.dimText

            font {
                family: "Scorek"
                pixelSize: 7
            }

            Text {
                color: parent.color
                x: -width - 0.1
                text: index > 0 ? alterText(index) : ""

                font {
                    family: "Scorek"
                    pixelSize: 7
                }

            }

            Behavior on scale {
                enabled: GLOB.useAnimations

                NumberAnimation {
                }

            }

        }

    }

    MouseArea {
        id: ma

        anchors.fill: parent
        hoverEnabled: !NOO.isAndroid()
        onClicked: importWindow.showChord(chordIt)
    }

}
