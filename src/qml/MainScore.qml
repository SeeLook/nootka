/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0
import Nootka 1.0
import Nootka.Main 1.0
import "score"


Score {
  id: mainScore

  property alias showNamesAct: mainObj.showNamesAct
  property alias extraAccidsAct: mainObj.extraAccidsAct
  property alias zoomInAct: mainObj.zoomInAct
  property alias zoomOutAct: mainObj.zoomOutAct
  property alias openXmlAct: mainObj.openXmlAct
  property alias saveXmlAct: mainObj.saveXmlAct
  property alias playAct: mainObj.playAct
  property alias scoreActions: mainObj.scoreActions
  property alias melodyActions: mainObj.melodyActions
  property alias noteActions: mainObj.noteActions
  property alias notesMenuAct: mainObj.notesMenuAct
  property alias randMelodyAct: mainObj.randMelodyAct
  property alias keyName: keyName

  scoreObj.meter: GLOB.rhythmsEnabled && !GLOB.singleNoteMode ? Tmeter.Meter_4_4 : Tmeter.NoMeter
  focus: true

  clef: GLOB.clefType
  enableDoubleAccids: GLOB.enableDoubleAccids
  scoreObj.showNoteNames: GLOB.namesOnScore
  scoreObj.nameColor: GLOB.nameColor
  scoreObj.nameStyle: GLOB.noteNameStyle
  scoreObj.enableDoubleAccidentals: GLOB.enableDoubleAccids
  scoreObj.enharmNotesEnabled: GLOB.showEnharmNotes
  scoreObj.bgColor: activPal.base
  scoreObj.enableTechnical: GLOB.instrument.bandoneon

  TmainScoreObject {
    id: mainObj
    onMelodyGenerate: nootkaWindow.showDialog(Nootka.MelodyGenerator)
    mainScoreItem: mainScore
    onMelodyNameDialog: Qt.createComponent("qrc:/score/MelodyNameDialog.qml").createObject(nootkaWindow)
    onWantSelectGotIt: Qt.createComponent("qrc:/gotit/NoteSelected.qml").createObject(nootkaWindow.contentItem.parent, { "remaindChecked": true })
    notesMenuAct.enabled: mainScore.meter !== Tmeter.NoMeter
  }

  Timer { id: zoomTimer; interval: 500 }

  PinchArea {
    z: -1
    anchors.fill: parent
    pinch.dragAxis: Pinch.XandYAxis
    onPinchFinished: {
      if (pinch.scale > 1.4)
        zoom(true)
      else if (pinch.scale < 0.7)
        zoom(false)
    }
    // HACK: keeping MouseArea inside PinchArea makes it working
    MouseArea {
      anchors.fill: parent
      //z: -1
      onWheel: {
        if (wheel.modifiers & Qt.ControlModifier) {
            if (wheel.angleDelta.y > 0)
              zoom(true)
            else if (wheel.angleDelta.y < 0)
              zoom(false)
        } else
            wheel.accepted = false
      }
      // reset note selection when score is clicked
      onClicked: currentNote = null
    }
  }

  function zoom(zoomIn) {
    if (!zoomTimer.running) {
      if (zoomIn)
        zoomInAct.trigger()
      else
        zoomOutAct.trigger()
      zoomTimer.running = true
    }
  }

  Text {
    id: keyName
    parent: firstStaff
    visible: GLOB.showKeyName && enableKeySign
    x: clef === Tclef.PianoStaffClefs ? 7.5 : 5.5
    y: clef === Tclef.PianoStaffClefs ? 4.2 : 7
    color: activPal.text
    font { pixelSize: 12 }
    transformOrigin: Item.TopLeft; scale: 1.0 / 6.0
    text: GLOB.showKeyName && enableKeySign ? mainObj.keyNameText : ""
  }

  // private
  property var scordature: null
  property var notePrompt: null

  Connections {
    target: SOUND
    onInitialized: {
      mainObj.scoreObject = scoreObj
      singleNote = Qt.binding(function() { return GLOB.singleNoteMode })
      scoreObj.allowAdding = Qt.binding(function() { return !GLOB.singleNoteMode })
      enableKeySign = Qt.binding(function() { return GLOB.keySignatureEnabled })
      updateScord()
      if (!GLOB.singleNoteMode)
        scoreObj.editModeAct.trigger()
      if (NOO.isAndroid() && GLOB.gotIt("howToScore", true))
        Qt.createComponent("qrc:/gotit/FirstTouchScore.qml").createObject(mainScore)
    }
  }

  Connections {
    target: GLOB
    onClefTypeChanged: score.clef = GLOB.clefType
  }

  Connections {
    target: GLOB.tuning
    onScordatureChanged: updateScord()
  }

  function updateScord() {
    if (scordature)
      scordature.destroy()
    if (GLOB.tuning.scordature && GLOB.instrument.isGuitar && GLOB.instrument.type !== Tinstrument.BassGuitar) {
        scordature = Qt.createComponent("qrc:/score/Scordature.qml").createObject(firstStaff)
        firstStaff.scordSpace = scordature.realHeight
    } else
        firstStaff.scordSpace = 0
  }

  onSingleNoteChanged: {
    if (singleNote)
      notePrompt = Qt.createComponent("qrc:/score/NotePrompt.qml").createObject(scoreObj.note(0))
    else
      notePrompt.destroy()
  }

  Rectangle { // note highlight
    id: noteHighlight
    parent: currentNote
    visible: currentNote != null && !singleNote
    width: currentNote ? (currentNote.width - currentNote.alterWidth) * 1.5 : 0
    height: currentNote ? Math.min(12.0, currentNote.notePosY + 6.0) : 0
    x: currentNote ? -width * 0.25 : 0
    y: currentNote ? Math.min(currentNote.height - height, Math.max(0.0, currentNote.notePosY - height / 2.0)) : 0
    color: NOO.alpha(activPal.highlight, 75)
    z: -1
    radius: width / 3.0
  }
}
