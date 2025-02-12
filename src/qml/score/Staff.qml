/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

pragma ComponentBehavior: Bound

import Nootka 1.0
import Nootka.Music
import QtQuick 2.12
import QtQuick.Window 2.12
import Score 1.0

TstaffItem {
    id: staff

    required property var score
    property alias clef: clef
    property real linesCount: score.clef === Tclef.PianoStaffClefs ? 49 : 38
    property var keySignItem: null
    property var meter: null
    property real firstNoteX: (meter ? meter.x + meter.width : (keySignItem ? keySignItem.x + keySignItem.width : clef.x + clef.width)) + 1

    function checkIsKeyEnabled() {
        // key signature created on demand
        if (score.enableKeySign) {
            if (!keySignItem)
                keySignItem = Qt.createComponent("qrc:/score/KeySignature.qml").createObject(staff);

        } else {
            if (keySignItem) {
                keySignItem.destroy();
                keySignItem = null;
            }
        }
    }

    height: linesCount
    scale: score.singleNote ? Math.min(score.height / linesCount, score.width / 46)
                            : (Math.min(Math.min(score.height, score.width / 2),
                                        Math.max(Screen.height / 4, Screen.pixelDensity * (NOO.isAndroid() ? 50 : 70))) / linesCount) * score.scaleFactor
    width: score.width / scale
    transformOrigin: Item.TopLeft
    scoreObject: score.scoreObj
    notesIndent: firstNoteX
    upperLine: score.clef === Tclef.PianoStaffClefs ? 14 : 16
    Component.onCompleted: checkIsKeyEnabled()

    TstaffLines {
        id: upperStaff

        x: staff.score.clef === Tclef.PianoStaffClefs ? 3 : 0.5
        width: staff.width - (staff.score.clef === Tclef.PianoStaffClefs ? 3.5 : 1)
        y: staff.upperLine - 0.1
        staffScale: staff.scale
    }

    Loader {
        sourceComponent: staff.score.clef === Tclef.PianoStaffClefs ? lowerStaff : null
    }

    Component {
        id: lowerStaff

        TstaffLines {
            x: staff.score.clef === Tclef.PianoStaffClefs ? 3 : 0.5
            width: staff.width - (staff.score.clef === Tclef.PianoStaffClefs ? 3.5 : 1)
            y: staff.upperLine - 0.1 + 22
            staffScale: staff.scale
        }

    }

    Loader {
        sourceComponent: staff.score.clef === Tclef.PianoStaffClefs ? brace : null
    }

    Component {
        id: brace

        Text {
            text: "\ue003"
            y: -13
            x: 3.1
            color: activPal.text

            font {
                family: "scorek"
                pixelSize: 21
            }
        }
    }

    Clef {
        id: clef
    }

    Connections {
        target: staff.score
        function onEnableKeySignChanged() : void { staff.checkIsKeyEnabled(); }
    }

    // measure number
    Text {
        x: staff.score.clef === Tclef.PianoStaffClefs ? 0.8 : 0.5
        y: staff.upperLine
            - (staff.score.clef === Tclef.Treble_G || score.clef === Tclef.Treble_G_8down
                || score.clef === Tclef.Tenor_C || score.clef === Tclef.PianoStaffClefs
                ? 8 : 5)
        text: staff.firstMeasureNr + 1
        visible: staff.number > 0 && staff.firstMeasureNr > 0
        scale: 0.4
        transformOrigin: Item.TopLeft
        color: activPal.text

        font {
            pixelSize: staff.score.clef === Tclef.PianoStaffClefs ? 7 : 5
            family: "Scorek"
        }

    }

}
