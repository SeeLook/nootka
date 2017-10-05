/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0
import Nootka 1.0


Score {
  id: mainScore

  width: parent.width

  property alias showNamesAct: showNamesAct
  property alias extraAccidsAct: extraAccidsAct
  property alias zoomInAct: zoomInAct
  property alias zoomOutAct: zoomOutAct
  property alias openXmlAct: openXmlAct
  property alias saveXmlAct: saveXmlAct
  property alias deleteLastAct: deleteLastAct
  property alias clearScoreAct: clearScoreAct
  property alias recModeAct: recModeAct
  property alias playAct: playAct

  scoreObj.meter: GLOB.rhythmsEnabled && !GLOB.singleNoteMode ? Tmeter.Meter_4_4 : Tmeter.NoMeter
  focus: true

  onFocusChanged: {
    if (!focus) // FIXME: space key is stolen if any dialog/popup is invoked, this workaround avoids that but side effects are unknown yet
      focus = true
  }

  clef: GLOB.clefType
  scoreObj.clefType: GLOB.clefType
  enableDoubleAccids: GLOB.enableDoubleAccids
  enableKeySign: GLOB.keySignatureEnabled
  scoreObj.showNoteNames: GLOB.namesOnScore
  scoreObj.nameColor: GLOB.nameColor
  scoreObj.nameStyle: GLOB.noteNameStyle
  scoreObj.enableDoubleAccidentals: GLOB.enableDoubleAccids
  scoreObj.allowAdding: !GLOB.singleNoteMode


  Timer { id: zoomTimer; interval: 500 }
  MouseArea {
    anchors.fill: parent
    z: -1
    onWheel: {
      if (wheel.modifiers & Qt.ControlModifier) {
        if (wheel.angleDelta.y > 0) {
            if (!zoomTimer.running) {
              zoomInAct.trigger()
              zoomTimer.running = true
            }
        } else if (wheel.angleDelta.y < 0) {
            if (!zoomTimer.running) {
              zoomOutAct.trigger()
              zoomTimer.running = true
            }
        }
      }
    }
  }

  Text {
    id: keyName
    parent: firstStaff
    visible: GLOB.showKeyName && enableKeySign
    x: clef === Tclef.PianoStaffClefs ? 6.5 : 4.5
    y: clef === Tclef.PianoStaffClefs ? 2.5 : 5
    color: activPal.text
    font { family: "Sans"; pointSize: 1.5 }
    text: getKeyNameText()
    function getKeyNameText() {
      return enableKeySign && firstStaff.keySignature ? Noo.majAndMinKeyName(firstStaff.keySignature.key) : ""
    }
  }

  Component.onCompleted: {
    scoreObj.singleNote = GLOB.singleNoteMode
  }

  Connections {
    target: GLOB
    onKeyNameChanged: keyName.text = Qt.binding(keyName.getKeyNameText)
    onSingleNoteModeChanged: {
      scoreObj.singleNote = GLOB.singleNoteMode
      if (GLOB.singleNoteMode) {
          recordMode = false
          scoreObj.note(1).visible = Qt.binding(function() { return GLOB.showEnharmNotes })
          scoreObj.note(2).visible = Qt.binding(function() { return GLOB.showEnharmNotes && GLOB.enableDoubleAccids })
      }
    }
  }

  Rectangle { // note highlight
    id: noteHighlight
    parent: currentNote
    visible: currentNote != null
    width: currentNote ? (currentNote.width - currentNote.alterWidth) * 1.5 : 0
    height: currentNote ? Math.min(12.0, currentNote.notePosY + 6.0) : 0
    x: currentNote ? -width * 0.25 : 0
    y: currentNote ? Math.min(currentNote.height - height, Math.max(0.0, currentNote.notePosY - height / 2.0)) : 0
    color: Qt.rgba(activPal.highlight.r, activPal.highlight.g, activPal.highlight.b, 0.3)
    z: -1
    radius: width / 3.0
  }

  Taction {
    id: playAct
    text: Noo.TR("QShortcut", "Play")
    icon: "playMelody"
    onTriggered: SOUND.playScore()
//     shortcut: Shortcut { sequence: "Space"; onActivated: recModeAct.triggered() }
  }
  Taction {
    id: recModeAct
    text: recordMode ? qsTr("Note by note") : qsTr("Edit")
    icon: recordMode ? "record" : "stopMelody"
    onTriggered: recordMode = !recordMode
//     shortcut: Shortcut { sequence: "Ctrl+Space"; onActivated: recModeAct.triggered() }
  }
  Taction {
    id: openXmlAct
    text: Noo.TR("QShortcut", "Open")
    icon: "open"
    onTriggered: {
      SOUND.stopListen()
      var xmlFle = Noo.getXmlToOpen()
      scoreObj.openMusicXml(xmlFle)
      SOUND.startListen()
    }
    shortcut: Shortcut { sequence: StandardKey.Open; onActivated: openXmlAct.triggered() }
  }
  Taction {
    id: saveXmlAct
    text: Noo.TR("QShortcut", "Save")
    icon: "save"
    onTriggered: {
      var xmlFle = Noo.getXmlToSave()
      scoreObj.saveMusicXml(xmlFle)
    }
    shortcut: Shortcut { sequence: StandardKey.Save; onActivated: saveXmlAct.triggered() }
  }
  Taction {
    id: extraAccidsAct
    enabled: !GLOB.singleNoteMode
    text: qsTr("Additional accidentals")
    checkable: true
  }
  Taction {
    id: showNamesAct
    enabled: !GLOB.singleNoteMode
    text: qsTr("Show note names")
    checkable: true
    checked: GLOB.namesOnScore
    onTriggered: { scoreObj.showNoteNames = checked }
  }
  Taction {
    id: zoomOutAct
    enabled: !GLOB.singleNoteMode
    icon: "zoom-out"
    text: qsTr("Zoom score out")
    onTriggered: scaleFactor = Math.max(0.4, scaleFactor - 0.2)
    shortcut: Shortcut { sequence: StandardKey.ZoomOut; onActivated: zoomOutAct.triggered() }
  }
  Taction {
    id: zoomInAct
    enabled: !GLOB.singleNoteMode
    icon: "zoom-in"
    text: qsTr("Zoom score in")
    onTriggered: scaleFactor = scaleFactor = Math.min(scaleFactor + 0.2, 1.4)
    shortcut: Shortcut { sequence: StandardKey.ZoomIn; onActivated: zoomInAct.triggered() }
  }
  Taction {
    id: deleteLastAct
    enabled: !GLOB.singleNoteMode
    icon: "delete"
    text: qsTr("Delete note")
    onTriggered: scoreObj.deleteLastNote()
    shortcut: Shortcut { sequence: "Del"; onActivated: deleteLastAct.triggered() }
  }
  Taction {
    id: clearScoreAct
    enabled: !GLOB.singleNoteMode
    icon: "clear-score"
    text: qsTr("Delete all notes")
    onTriggered: clearScore()
    shortcut: Shortcut { sequence: "Shift+Del"; onActivated: clearScoreAct.triggered() }
  }

  Shortcut {
    enabled: !GLOB.singleNoteMode
    sequence: StandardKey.MoveToNextChar;
    onActivated: {
      if (currentNote)
        currentNote = scoreObj.getNext(currentNote)
      else
        currentNote = scoreObj.note(0)
    }
  }
  Shortcut {
    enabled: !GLOB.singleNoteMode
    sequence: StandardKey.MoveToPreviousChar;
    onActivated: {
      if (currentNote)
        currentNote = scoreObj.getPrev(currentNote)
      else
        currentNote = scoreObj.note(notesCount - 1)
    }
  }
  Keys.onSpacePressed: {
    enabled: !GLOB.singleNoteMode
    if (event.modifiers & Qt.ControlModifier)
      recModeAct.triggered()
    else
      playAct.triggered()
  }

}
