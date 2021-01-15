/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0


Flickable {
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
  property alias recordMode: scoreObj.recordMode
  property alias bgRect: bgRect
  property alias alterText: scoreObj.alterText

  property alias insertNoteAct: scoreObj.insertNoteAct
  property alias deleteNoteAct: scoreObj.deleteNoteAct
  property alias clearScoreAct: scoreObj.clearScoreAct

  // private
  property var staves: [ staff0 ]
  property var noteAdd: null
  property var delControl: null
  property var cursor: null
  property var scoreToobox: null

  clip: true
  boundsBehavior: Flickable.StopAtBounds
  width: parent.width

  contentWidth: score.width

  ScrollBar.vertical: ScrollBar { active: false; visible: active }

//   maximumFlickVelocity: 1000 // 2500 by default

  TscoreObject {
    id: scoreObj
    width: score.width / scale
    enableDoubleAccidentals: score.enableDoubleAccids

    onClicked: currentNote = scoreObj.activeNote

    onStaffCreate: {
      staves.push(Qt.createComponent("qrc:/score/Staff.qml").createObject(score.contentItem))
      score.lastStaff = staves[staves.length - 1]
    }
    onStavesHeightChanged: score.contentHeight = Math.max(stavesHeight, score.height)
    onStaffDestroying: { staves.splice(staffNr, 1); lastStaff = staves[staves.length - 1] }
    onNoteWasAdded: {
      if (staves.length > 1)
        ensureVisible(lastNote.staffItem.y, lastNote.staffItem.height * scale)
    }
    onAllowAddingChanged: {
      if (allowAdding) {
        if (!delControl)
          delControl = Qt.createComponent("qrc:/score/DelControl.qml").createObject(contentItem)
        if (!noteAdd)
          noteAdd = Qt.createComponent("qrc:/score/NoteAdd.qml").createObject(contentItem)
        if (!scoreToobox)
          scoreToobox = Qt.createComponent("qrc:/score/ScoreToolbox.qml").createObject(parent)
      }
    }
    onActiveNoteChanged: {
      if (!cursor) {
        cursor = Qt.createComponent("qrc:/score/ScoreCursor.qml").createObject(contentItem)
        cursor.parent = Qt.binding(function() { return scoreObj.activeNote })
      }
      if (!scoreToobox && !readOnly)
        scoreToobox = Qt.createComponent("qrc:/score/ScoreToolbox.qml").createObject(parent)
    }
  }

  onCurrentNoteChanged: {
    if (currentNote && staves.length > 1)
      ensureVisible(currentNote.staffItem.y, currentNote.staffItem.height * scale)
  }

  Rectangle { // entire score background & frame when record
    id: bgRect
    parent: score
    z: -1
    width: score.width; height: score.height
    color: Noo.alpha(scoreObj.bgColor, 230)
    border { width: recordMode ? 2 : 0; color: "red" }
  }

  Staff { // first staff (always exists)
    id: staff0
    meter: Meter { parent: staff0 }
  }

  Text { // edit mode symbol
    opacity: scoreObj.editMode && !singleNote ? 1 : 0
    Behavior on opacity { enabled: GLOB.useAnimations; NumberAnimation { duration: 500 }}
    x: Noo.fontSize() * 3; y: score.contentY + Noo.fontSize() / 2
    text: "\u0080"; font { family: "Nootka"; pixelSize: Noo.fontSize() * 2 }
    color: Noo.alpha(GLOB.noteCursorColor, 200)
  }

  function ensureVisible(yy, hh) {
    if (contentY >= yy)
      contentY = yy
    else if (contentY + height <= yy + hh)
      contentY = yy + hh - height
  }

  function addNote(n) { scoreObj.addNote(n ,true) }

  function setNote(noteItem, note) { scoreObj.setNote(noteItem, note) }

  function clearScore() { scoreObj.clearScore() }

  function deleteLast() { scoreObj.deleteLastNote() }
}
