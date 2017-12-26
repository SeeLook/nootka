/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import Score 1.0


Item {
  id: staff

  property alias clef: clef

  property alias upperLine: staffObj.upperLine
  property real linesCount: score.clef === Tclef.PianoStaffClefs ? 44 : 38
  property var keySignItem: null
  property var meter: null
  property real firstNoteX: (meter ? meter.x + meter.width : (keySignItem ? keySignItem.x + keySignItem.width : clef.x + clef.width)) + 1.0
  property alias scordSpace: staffObj.scordSpace

  signal destroing(var nr)

  height: linesCount
  scale: score.singleNote ? score.height / linesCount :
                            (Math.min(score.height, Math.max(Screen.height / 4, Screen.pixelDensity * 70)) / linesCount) * score.scaleFactor
  width: score.width / scale
  transformOrigin: Item.TopLeft

  TstaffObject {
      id: staffObj
      scoreObject: scoreObj
      staffItem: staff; notesIndent: firstNoteX
      upperLine: score.clef === Tclef.PianoStaffClefs ? 14 : 16
  }

  TstaffLines {
      id: upperStaff
      x: score.clef === Tclef.PianoStaffClefs ? 1.5 : 0.5
      width: staff.width - (score.clef === Tclef.PianoStaffClefs ? 2 : 1)
      y: staffObj.upperLine - 0.1
      staffScale: staff.scale
  }

  Loader { sourceComponent: score.clef === Tclef.PianoStaffClefs ? lowerStaff : null }
  Component {
      id: lowerStaff
      TstaffLines {
        x: score.clef === Tclef.PianoStaffClefs ? 1.5 : 0.5
        width: staff.width - (score.clef === Tclef.PianoStaffClefs ? 2 : 1)
        y: staffObj.upperLine - 0.1 + 14
        staffScale: staff.scale
      }
  }

  Loader { sourceComponent: score.clef === Tclef.PianoStaffClefs ? brace : null }
  Component {
      id: brace
      Text {
        visible: score.clef === Tclef.PianoStaffClefs
        text: "\ue000"
        font {family: "scorek"; pixelSize: 22 }
        y: -9
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
      y: staffObj.upperLine - (score.clef === Tclef.Treble_G || score.clef === Tclef.Treble_G_8down 
                              || score.clef === Tclef.Tenor_C || score.clef === Tclef.PianoStaffClefs ? 8.5 : 6)
      text: staffObj.firstMeasureNr + 1
      visible: staffObj.number > 0 && staffObj.firstMeasureNr > 0
      font { pixelSize: score.clef === Tclef.PianoStaffClefs ? 2 : 1.5; family: "Scorek" }
      color: activPal.text
  }

  function checkIsKeyEnabled() { // key signature created on demand
    if (score.enableKeySign) {
        if (!keySignItem) {
          var c = Qt.createComponent("qrc:/KeySignature.qml")
          keySignItem = c.createObject(staff)
        }
    } else {
        if (keySignItem) {
          keySignItem.destroy()
          keySignItem = null
        }
    }
  }

  Component.onCompleted: checkIsKeyEnabled()
  Component.onDestruction: destroing(staffObj.number)
}
