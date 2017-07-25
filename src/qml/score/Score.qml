/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0


Flickable {
  id: score

  property alias scoreObj: scoreObj
  property alias scale: staff0.scale
  property alias firstStaff: staff0
  property Staff lastStaff: staff0
  property int clef: Tclef.Treble_G
  property alias upperLine: staff0.upperLine
  property alias meter: scoreObj.meter
  property alias bgColor: bgRect.color
  property bool enableKeySign: scoreObj.keySignatureEnabled
  property bool enableDoubleAccids: false
  property alias workRhythm: scoreObj.workRhythm
  property real scaleFactor: 1.0
  property alias notesCount: scoreObj.notesCount
  property TnoteItem currentNote: null
  property alias readOnly: scoreObj.readOnly

  // private
  property var staves: []
  property alias noteAdd: addLoad.item

  clip: true
  boundsBehavior: Flickable.StopAtBounds
  width: parent.width

  contentWidth: score.width

  TscoreObject {
    id: scoreObj
    width: score.width / scale
    cursorAlter: accidControl.alter
    enableDoubleAccidentals: score.enableDoubleAccids

    onClicked: currentNote = scoreObj.activeNote

    onStaffCreate: {
      var c = Qt.createComponent("qrc:/Staff.qml")
      var lastStaff = c.createObject(score.contentItem, { "clef.type": score.clef })
      staves.push(lastStaff)
      lastStaff.enableKeySignature(enableKeySign && score.clef !== Tclef.NoClef)
      lastStaff.onDestroing.connect(removeStaff)
      if (enableKeySign && score.clef !== Tclef.NoClef) {
        lastStaff.keySignature.onKeySignatureChanged.connect(setKeySignature)
        lastStaff.keySignature.key = staff0.keySignature.key
      }
      score.lastStaff = lastStaff
    }
    onStavesHeightChanged: score.contentHeight = Math.max(stavesHeight, score.height)
    onKeySignatureChanged: setKeySignature(scoreObj.keySignature)
    onClefTypeChanged: staff0.clef.type = clefType
    onLastNoteChanged: { // occurs when meter changes, so rhythm control has to be set accordingly
      if (rtmControl && meter !== Tmeter.NoMeter) {
          rtmControl.rtm = meter <= Tmeter.Meter_7_4 ? Trhythm.Quarter : Trhythm.Eighth
          rtmControl.dot = false
          rtmControl.rest = false
          rtmControl.triplet = false
          workRhythm = rtmControl.rhythm
      } else
          workRhythm = Noo.rhythm(Trhythm.NoRhythm, false, false, false)
      if (noteAdd)
        noteAdd.lastNote = lastNote
    }

    function removeStaff(nr) { staves.splice(nr, 1); lastStaff = staves[staves.length - 1] }
  }

  onCurrentNoteChanged: {
    if (currentNote && staves.length > 1)
      ensureVisible(currentNote.staffItem.y, currentNote.staffItem.height * scale)
  }

  Rectangle { // entire score background
    id: bgRect
    anchors.fill: score.contentItem
    color: Qt.rgba(activPal.base.r, activPal.base.g, activPal.base.b, 0.9)
  }

  Staff { // first staff (always exists)
    id: staff0
    clef.type: score.clef
    meter: Meter { parent: staff0 }
    Component.onCompleted: staves.push(staff0)
  }

  NoteCursor {
    id: cursor
    parent: scoreObj.activeNote
    yPos: scoreObj.activeYpos
    alterText: accidControl.text
    headText: parent ? scoreObj.activeRtmText() : ""
  }

  AccidControl {
    id: accidControl
    active: !readOnly && score.clef !== Tclef.NoClef && (scoreObj.activeNote !== null || (noteAdd && noteAdd.active))
    visible: !scoreObj.touched
  }

  RhythmControl {
    id: rtmControl
    active: !readOnly && meter !== Tmeter.NoMeter && (scoreObj.activeNote !== null || (noteAdd && noteAdd.active))
    onChanged: scoreObj.workRhythm = rhythm
    visible: !scoreObj.touched
  }

  Loader { sourceComponent: scoreObj.allowAdding ? delControl : null }
  Component {
    id: delControl
    DelControl { active: !readOnly && scoreObj.activeNote !== null && scoreObj.activeNote === scoreObj.lastNote }
  }

  Loader { id: addLoad; sourceComponent: scoreObj.allowAdding ? addComp : null }
  Component {
    id: addComp
    NoteAdd {
      noteText: Noo.rhythmText(scoreObj.workRhythm)
      onAddNote: { score.addNote(scoreObj.posToNote(yPos)); currentNote = null }
      alterText: accidControl.text
    }
  }

  onEnableKeySignChanged: {
    staff0.enableKeySignature(enableKeySign)
    if (enableKeySign)
      staff0.keySignature.onKeySignatureChanged.connect(setKeySignature)
    for (var s = 1; s < staves.length; ++s) {
      staves[s].enableKeySignature(enableKeySign)
      if (enableKeySign)
        staff0.keySignature.onKeySignatureChanged.connect(setKeySignature)
    }
    scoreObj.keySignatureEnabled = enableKeySign
  }

  function staffChangesClef(staffId) {
    var newClef = staffId.clef.type
    if (newClef !== score.clef) {
//       score.clef = newClef
      for (var s = 0; s < staves.length; ++s) {
        if (staffId !== staves[s])
          staves[s].clef.type = newClef
        staves[s].enableKeySignature(newClef !== Tclef.NoClef && enableKeySign)
      }
      scoreObj.clefType = newClef
      score.clef = newClef
    }
  }

  function setKeySignature(key) {
    if (enableKeySign) {
      for (var s = 0; s < staves.length; ++s) {
        if (key !== staves[s].keySignature.key)
          staves[s].keySignature.key = key
      }
      scoreObj.keySignature = key
    }
  }

  function ensureVisible(yy, hh) {
    if (contentY >= yy)
      contentY = yy;
    else if (contentY + height <= yy + hh)
      contentY = yy + hh - height;
  }

  function addNote(n) {
    scoreObj.addNote(n)
    var lastNote = scoreObj.lastNote
    if (staves.length > 1)
      ensureVisible(lastNote.staffItem.y, lastNote.staffItem.height * scale)
    if (noteAdd)
      noteAdd.lastNote = lastNote
  }

  function setNote(noteItem, note) { scoreObj.setNote(noteItem, note) }

  function clearScore() {
    scoreObj.clearScore()
    currentNote = null
    accidControl.show = false
    rtmControl.show = false
  }
}
