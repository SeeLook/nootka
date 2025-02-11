/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Nootka.Music
import QtQuick 2.12
import Score 1.0

Item {
    id: noteCursor

    property string headText: parent && (score.singleNote || scoreObj.workRtmValue) ? scoreObj.activeRtmText() : "\uf4be"
    property color color: GLOB.noteCursorColor
    property real yPos: scoreObj.activeYpos

    height: parent ? parent.height : 0
    width: parent ? parent.width : 0
    visible: yPos > 0 && (GLOB.singleNote || scoreObj.editMode)

    // highlight
    Rectangle {
        width: parent.width + 1
        height: parent.height
        x: -1
        color: NOO.alpha(noteCursor.color, 26)
        z: -10
        radius: width / 8
    }

    Text {
        id: head

        scale: 1.2
        text: headText
        y: yPos - 14
        color: noteCursor.color
        x: score.singleNote ? 1.5 : 0

        font {
            family: "Scorek"
            pixelSize: 7
        }

        Text {
            id: alter

            color: noteCursor.color
            x: -width - 0.1
            text: score.alterText

            font {
                family: "Scorek"
                pixelSize: 7
            }

            Text {
                color: activPal.shadow
                x: 0.25
                y: 0.25
                z: -1
                text: parent.text
                opacity: 0.6

                font {
                    family: "Scorek"
                    pixelSize: 7
                }

            }

        }

        Text {
            x: 0.25
            y: 0.25
            z: -1
            color: activPal.shadow
            text: head.text
            opacity: 0.6

            font {
                family: "Scorek"
                pixelSize: 7
            }

        }

    }

    // upper lines
    Repeater {
        model: (score.upperLine - 2) / 2

        AddLine {
            y: 2 * (index + 1) - 0.1
            visible: yPos > 0 && index >= Math.floor((yPos - 1) / 2)
        }

    }

    // upper staff mid lines
    Repeater {
        model: score.clef === Tclef.PianoStaffClefs ? 2 : 0

        AddLine {
            y: score.upperLine + 10 + 2 * index - 0.1
            visible: yPos >= y && yPos < score.upperLine + 14
        }

    }

    // lower staff mid lines
    Repeater {
        model: score.clef === Tclef.PianoStaffClefs ? 4 : 0

        AddLine {
            y: score.upperLine + 14 + 2 * index - 0.1
            visible: yPos <= y + 0.1 && yPos > score.upperLine + 13 && yPos < score.upperLine + 24
        }

    }

    // lower lines
    Repeater {
        model: Math.max(0, (score.firstStaff.height - score.upperLine - (score.clef === Tclef.PianoStaffClefs ? 28 : 12)) / 2)

        AddLine {
            y: score.upperLine + (score.clef === Tclef.PianoStaffClefs ? 30 : 10) + 2 * index - 0.1
            visible: score.upperLine + (score.clef === Tclef.PianoStaffClefs ? 30 : 10) + index * 2 <= yPos
        }

    }

}
