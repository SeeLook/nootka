/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Window 2.0

import Score 1.0


Item {
  id: staff

  property alias clef: clef

  property alias upperLine: staffObj.upperLine
  property real linesCount: clef.type === Tclef.PianoStaffClefs ? 44 : 38
  property KeySignature keySignature: null
  property Meter meter: null
  property real firstNoteX: (meter ? meter.x + meter.width : (keySignature ? keySignature.x + keySignature.width : 0.5 + clef.width)) + 1.0

  signal destroing(var nr)

  height: linesCount
  scale: (Math.min(score.height, Math.max(Screen.height / 4, Screen.pixelDensity * 70)) / linesCount) * score.scaleFactor
  width: score.width / scale
  transformOrigin: Item.TopLeft

  TstaffObject { id: staffObj; score: scoreObj; staffItem: staff; notesIndent: firstNoteX }

  TstaffLines {
      id: upperStaff
      x: clef.type === Tclef.PianoStaffClefs ? 1.5 : 0.5
      width: staff.width - (clef.type === Tclef.PianoStaffClefs ? 2 : 1)
      y: staffObj.upperLine - 0.1
      staffScale: staff.scale
  }

  Loader { sourceComponent: clef.type === Tclef.PianoStaffClefs ? lowerStaff : null }
  Component {
      id: lowerStaff
      TstaffLines {
        x: clef.type === Tclef.PianoStaffClefs ? 1.5 : 0.5
        width: staff.width - (clef.type === Tclef.PianoStaffClefs ? 2 : 1)
        y: staffObj.upperLine - 0.1 + 14
        staffScale: staff.scale
      }
  }

  Loader { sourceComponent: clef.type === Tclef.PianoStaffClefs ? brace : null }
  Component {
      id: brace
      Text {
        visible: clef.type === Tclef.PianoStaffClefs
        text: "\ue000"
        font {family: "scorek"; pixelSize: 22 }
        y: -9
        color: activPal.text
      }
  }

  Clef {
      id: clef
      onTypeChanged: {
        staffObj.upperLine = clef.type === Tclef.PianoStaffClefs ? 14 : 16
        if (keySignature) {
          keySignature.changeClef(clef.type)
          if (meter)
            updateMeterPos()
        }
        score.staffChangesClef(staff)
      }
  }

  Text { // measure number
      x: clef.x
      y: staffObj.upperLine - (clef.type === Tclef.Treble_G || clef.type === Tclef.Treble_G_8down 
                              || clef.type === Tclef.Tenor_C || clef.type === Tclef.PianoStaffClefs ? 6 : 3)
      text: staffObj.firstMeasureNr + 1
      visible: staffObj.number > 0 && staffObj.firstMeasureNr > 0
      font.pixelSize: 2
      color: activPal.text
  }

  function enableKeySignature(en) { // key signature created on demand
      if (en) {
          if (!keySignature) {
            var c = Qt.createComponent("qrc:/KeySignature.qml")
            keySignature = c.createObject(staff)
            keySignature.changeClef(clef.type)
            if (meter) {
              keySignature.onWidthChanged.connect(updateMeterPos)
              updateMeterPos()
            }
          }
      } else {
          if (keySignature) {
            keySignature.destroy()
            keySignature = null
          }
          if (meter)
            meter.x = clef.x + clef.width
      }
  }

  function updateMeterPos() {
      meter.x = keySignature.x + keySignature.width
  }

  Component.onDestruction: { destroing(staffObj.number) }
}
