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

  property real linesCount: 40
  property KeySignature keySignature: null
  property Meter meter: null
  property real firstNoteX: (meter ? meter.x + meter.width : (keySignature ? keySignature.x + keySignature.width : 0.5 + clef.width)) + 1.0

  signal destroing(var nr)

  height: linesCount
  scale: (Math.min(score.height, Math.max(Screen.height / 4, Screen.pixelDensity * 70)) / linesCount) * score.scaleFactor
  width: score.width / scale
  transformOrigin: Item.TopLeft

  TstaffObject { id: staffObj; score: scoreObj; staffItem: staff; notesIndent: firstNoteX }

  Repeater { // staff lines
      model: 5
      Rectangle {
        x: 0.5
        y: staffObj.upperLine + 2 * index - 0.1
        height: 0.2
        width: staff.width - 1.0
        color: activPal.text
      }
  }

  Clef {
      id: clef
      onTypeChanged: {
        if (keySignature)
          keySignature.changeClef(clef.type)
        // TODO: approve clef change to the notes
      }
  }

  Text { // measure number
      x: 1
      y: staffObj.upperLine - (clef.type === Tclef.Treble_G || clef.type === Tclef.Treble_G_8down || clef.type === Tclef.Tenor_C ? 6 : 3)
      text: staffObj.firstMeasureNr + 1
      visible: staffObj.number > 0 && staffObj.firstMeasureNr > 0
      font.pixelSize: 2
      color: activPal.text
  }

  function enableKeySignature(en) { // key signature created on demand
      if (en) {
          if (!keySignature) {
            var c = Qt.createComponent("qrc:/KeySignature.qml")
            keySignature = c.createObject(staff, { x: clef.x + clef.width + 1 })
            keySignature.changeClef(clef.type)
            if (meter) {
              keySignature.onWidthChanged.connect(updateMeterPos)
              updateMeterPos()
            }
          }
      } else {
          if (keySignature)
            keySignature.destroy()
          if (meter)
            meter.x = clef.x + clef.width
      }
  }

  function updateMeterPos() {
    meter.x = keySignature.x + keySignature.width
  }
  Component.onDestruction: destroing(staffObj.number)
}
