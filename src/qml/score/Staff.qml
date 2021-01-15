/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import Score 1.0


TstaffItem {
  id: staff

  property alias clef: clef

  property real linesCount: score.clef === Tclef.PianoStaffClefs ? 49 : 38
  property var keySignItem: null
  property var meter: null
  property real firstNoteX: (meter ? meter.x + meter.width : (keySignItem ? keySignItem.x + keySignItem.width : clef.x + clef.width)) + 1.0

  height: linesCount
  scale: score.singleNote ? Math.min(score.height / linesCount, score.width / 46) :
                            (Math.min(score.height, Math.max(Screen.height / 4, Screen.pixelDensity * 70)) / linesCount) * score.scaleFactor
  width: score.width / scale
  transformOrigin: Item.TopLeft

  scoreObject: scoreObj
  notesIndent: firstNoteX
  upperLine: score.clef === Tclef.PianoStaffClefs ? 14 : 16

  TstaffLines {
      id: upperStaff
      x: score.clef === Tclef.PianoStaffClefs ? 3 : 0.5
      width: staff.width - (score.clef === Tclef.PianoStaffClefs ? 3.5 : 1)
      y: upperLine - 0.1
      staffScale: staff.scale
  }

  Loader { sourceComponent: score.clef === Tclef.PianoStaffClefs ? lowerStaff : null }
  Component {
      id: lowerStaff
      TstaffLines {
        x: score.clef === Tclef.PianoStaffClefs ? 3 : 0.5
        width: staff.width - (score.clef === Tclef.PianoStaffClefs ? 3.5 : 1)
        y: upperLine - 0.1 + 22
        staffScale: staff.scale
      }
  }

  Loader { sourceComponent: score.clef === Tclef.PianoStaffClefs ? brace : null }
  Component {
      id: brace
      Text {
        text: "\ue003"
        font {family: "scorek"; pixelSize: 21 }
        y: -14; x: 3.1
        color: activPal.text
      }
  }

  Clef { id: clef }

  Connections {
    target: score
    onEnableKeySignChanged: checkIsKeyEnabled()
  }

  Text { // measure number
      x: 1
      y: upperLine - (score.clef === Tclef.Treble_G || score.clef === Tclef.Treble_G_8down 
                              || score.clef === Tclef.Tenor_C || score.clef === Tclef.PianoStaffClefs ? 8.5 : 6)
      text: firstMeasureNr + 1
      visible: number > 0 && firstMeasureNr > 0
      font { pixelSize: score.clef === Tclef.PianoStaffClefs ? 2 : 1.5; family: "Scorek" }
      color: activPal.text
  }

  function checkIsKeyEnabled() { // key signature created on demand
    if (score.enableKeySign) {
        if (!keySignItem)
          keySignItem = Qt.createComponent("qrc:/score/KeySignature.qml").createObject(staff)
    } else {
        if (keySignItem) {
          keySignItem.destroy()
          keySignItem = null
        }
    }
  }

  Component.onCompleted: checkIsKeyEnabled()
}
