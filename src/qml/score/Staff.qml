/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
 *   seelook@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0

import Score 1.0


Item {
  id: staff

  property alias clef: clef

  property real linesCount: 40
  property int number: -1
  property KeySignature keySignature: null
  property Meter meter: null
  property real firstNoteX: (meter ? meter.x + meter.width : (keySignature ? keySignature.x + keySignature.width : 0.5 + clef.width)) + 1.0

  height: linesCount
  scale: score.height / linesCount
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
      visible: number > 0 && staffObj.firstMeasureNr > 0
      font.pixelSize: 2
      color: activPal.text
  }

  function enableKeySignature(en) { // key signature created on demand
      if (en) {
          if (!keySignature) {
            var c = Qt.createComponent("qrc:/KeySignature.qml")
            keySignature = c.createObject(staff, { x: clef.x + clef.width + 1 })
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
}
