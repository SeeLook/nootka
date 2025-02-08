/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import Nootka.Music
import QtQuick 2.12
import QtQuick.Controls 2.12
import Score 1.0

Text {
    id: clef

    signal clicked(var cl)

    function getPos() {
        x = 0.5;
        switch (score.clef) {
        case Tclef.Treble_G:
        case Tclef.Treble_G_8down:
            y = 5;
            break;
        case Tclef.Bass_F:
        case Tclef.Bass_F_8down:
        case Tclef.Tenor_C:
            y = 1;
            break;
        case Tclef.Alto_C:
            y = 3;
            break;
        case Tclef.PianoStaffClefs:
            y = 3;
            x = 3;
            break;
        case Tclef.NoClef:
            y = 3;
            x = 2;
            break;
        }
    }

    width: 5.5
    x: 0.5
    y: 5
    text: NOO.clef(score.clef).glyph()
    color: clefArea.containsMouse && (GLOB.singleNoteMode || scoreObj.editMode) ? GLOB.noteCursorColor : activPal.text
    Component.onCompleted: getPos()

    font {
        family: "Scorek"
        pixelSize: 8
    }

    MouseArea {
        id: clefArea

        property var clefDrawer

        y: 5
        width: parent.width
        height: parent.height + (score.clef === Tclef.PianoStaffClefs ? 4 : -12)
        enabled: !score.readOnly && !GLOB.isExam && (GLOB.singleNoteMode || scoreObj.editMode)
        hoverEnabled: GLOB.singleNoteMode || scoreObj.editMode
        onClicked: {
            if (clefDrawer) {
                clefDrawer.open();
            } else {
                clefDrawer = Qt.createComponent("qrc:/score/ClefDrawer.qml").createObject(nootkaWindow.contentItem);
                clefDrawer.clefMenu.clicked.connect(clef.clicked);
            }
            clefDrawer.selectedClef = score.clef;
        }
    }

    Loader {
        sourceComponent: score.clef === Tclef.PianoStaffClefs ? lowerClef : null
    }

    Component {
        id: lowerClef

        // bass clef at lower staff
        Text {
            font: clef.font
            text: "\ue062"
            color: clef.color
            y: clef.y + 15
        }

    }

    Connections {
        target: score
        function onClefChanged() : void { getPos() }
    }

}
