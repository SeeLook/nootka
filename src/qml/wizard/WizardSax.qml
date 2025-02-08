/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import "../"
import Nootka 1.0
import QtQuick 2.12

Column {
    id: saxCol

    property alias transposition: transpose.outShift

    spacing: NOO.factor()
    anchors.horizontalCenter: parent.horizontalCenter
    width: parent.width - NOO.factor() * 2

    Text {
        width: parent.width
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        color: activPal.text
        text: qsTr("Saxophones are transposing instruments.<br>It means that note pitch in the score doesn't correspond directly to note which is played - it transposes by interval (number of semitones) according to kind of saxophone.")

        font {
            pixelSize: NOO.factor() * 1.2
            bold: true
        }

    }

    Tile {
        width: transCol.width + NOO.factor() * 2
        bgColor: nootkaWindow.bgColor

        bgBorder {
            width: 2
            color: nootkaWindow.labelColor
        }

        Column {
            id: transCol

            width: Math.min(NOO.factor() * 50, saxCol.width * 0.8)
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: NOO.factor()

            Transposition {
                id: transpose

                anchors.horizontalCenter: parent.horizontalCenter
                shift: NOO.instr(nootkaWindow.instrument).transposition
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: NOO.noteName(NOO.note(13 + transpose.outShift, false), 0, false)
                color: activPal.text

                font {
                    pixelSize: NOO.factor() * 2
                    bold: true
                }

            }

        }

    }

    Text {
        text: "Saxophone support is new and not well tested,\nso please give some feedback or call a bug."
        color: "red"
        width: parent.width
        horizontalAlignment: Text.AlignHCenter

        font {
            pixelSize: NOO.factor() * 1.2
            bold: true
        }

    }

    Connections {
        target: nootkaWindow
        function onInstrumentChanged() : void { transpose.shift = NOO.instr(nootkaWindow.instrument).transposition }
    }

}
