/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import Nootka 1.0
import QtQuick 2.12
import QtQuick.Controls 2.12
import Score 1.0

Flickable {
    //   maximumFlickVelocity: 1000 // 2500 by default

    id: score

    property alias scoreObj: scoreObj
    property alias scale: staff0.scale
    property alias firstStaff: staff0
    property Staff lastStaff: staff0
    property alias clef: scoreObj.clefType
    property alias upperLine: staff0.upperLine
    property alias meter: scoreObj.meter
    property alias bgColor: bgRect.color
    property alias enableKeySign: scoreObj.keySignatureEnabled
    property alias keySignature: scoreObj.keySignature
    property bool enableDoubleAccids: false
    property alias workRhythm: scoreObj.workRhythm
    property alias scaleFactor: scoreObj.scaleFactor
    property alias notesCount: scoreObj.notesCount
    property alias currentNote: scoreObj.selectedItem
    property alias note: scoreObj.selectedNote
    property alias readOnly: scoreObj.readOnly
    property alias singleNote: scoreObj.singleNote
    property alias bgRect: bgRect
    property alias alterText: scoreObj.alterText
    property alias editText: editText
    property alias insertNoteAct: scoreObj.insertNoteAct
    property alias deleteNoteAct: scoreObj.deleteNoteAct
    property alias clearScoreAct: scoreObj.clearScoreAct
    // private
    property var staves: [staff0]
    property var noteAdd: null
    property var delControl: null
    property var cursor: null
    property var scoreToobox: null

    function ensureVisible(yy, hh) {
        if (contentY >= yy)
            contentY = yy;
        else if (contentY + height <= yy + hh)
            contentY = yy + hh - height;
    }

    function addNote(n) {
        scoreObj.addNote(n, true);
    }

    function setNote(noteItem, note) {
        scoreObj.setNote(noteItem, note);
    }

    function clearScore() {
        scoreObj.clearScore();
    }

    function deleteLast() {
        scoreObj.deleteLastNote();
    }

    clip: true
    boundsBehavior: Flickable.StopAtBounds
    width: parent.width
    contentWidth: score.width
    onCurrentNoteChanged: {
        if (currentNote && staves.length > 1)
            ensureVisible(currentNote.staffItem.y, currentNote.staffItem.height * scale);

    }

    TscoreObject {
        id: scoreObj

        width: score.width / scale
        enableDoubleAccidentals: score.enableDoubleAccids
        onClicked: currentNote = scoreObj.activeNote
        onStaffCreate: {
            staves.push(Qt.createComponent("qrc:/score/Staff.qml").createObject(score.contentItem));
            score.lastStaff = staves[staves.length - 1];
        }
        onStavesHeightChanged: score.contentHeight = Math.max(stavesHeight, score.height)
        onStaffDestroying: staffNr => {
            staves.splice(staffNr, 1);
            lastStaff = staves[staves.length - 1];
        }
        onNoteWasAdded: {
            if (staves.length > 1)
                ensureVisible(lastNote.staffItem.y, lastNote.staffItem.height * scale);

        }
        onAllowAddingChanged: {
            if (allowAdding) {
                if (!delControl)
                    delControl = Qt.createComponent("qrc:/score/DelControl.qml").createObject(contentItem);
                if (!noteAdd)
                    noteAdd = Qt.createComponent("qrc:/score/NoteAdd.qml").createObject(contentItem);
                if (!scoreToobox)
                    scoreToobox = Qt.createComponent("qrc:/score/ScoreToolbox.qml").createObject(score);

            }
        }
        onActiveNoteChanged: {
            if (!cursor) {
                cursor = Qt.createComponent("qrc:/score/ScoreCursor.qml").createObject(contentItem);
                cursor.parent = Qt.binding(function() {
                    return scoreObj.activeNote;
                });
            }
            if (!scoreToobox && !readOnly)
                scoreToobox = Qt.createComponent("qrc:/score/ScoreToolbox.qml").createObject(score);

        }
        onScoreWasCleared: ensureVisible(0, 0)
    }

    // entire score background
    Rectangle {
        id: bgRect

        parent: score
        z: -1
        width: score.width
        height: score.height
        color: NOO.alpha(scoreObj.bgColor, 230)
    }

    // first staff (always exists)
    Staff {
        id: staff0

        meter: Meter {
            parent: staff0
        }

    }

    // edit mode symbol
    Text {
        id: editText

        opacity: scoreObj.editMode && !singleNote ? 1 : 0
        x: NOO.factor() * 3
        y: score.contentY + NOO.factor() / 2
        text: "\u0080"
        color: NOO.alpha(GLOB.noteCursorColor, 200)

        font {
            family: "Nootka"
            pixelSize: NOO.factor() * 2
        }

        Behavior on opacity {
            enabled: GLOB.useAnimations

            NumberAnimation {
                duration: 500
            }

        }

    }

    ScrollBar.vertical: ScrollBar {
        active: false
        visible: active
    }

}
