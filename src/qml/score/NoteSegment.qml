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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0

import Score 1.0


Item {
  id: noteSegment

  height: parent.height
  width: 7

  property TnoteObject noteObj: null
  property color color: activPal.text

  onNoteObjChanged: noteObj.changed.connect(update)

  Text {
      id: head
      font { family: "Scorek"; pixelSize: 7 }
      y: noteObj.notePosY - 15
      color: noteSegment.color
      visible: noteObj.notePosY > 0
  }

  Text {
      id: alter
      font { family: "Scorek"; pixelSize: 7 }
      y: noteObj.notePosY - 15
      color: noteSegment.color
      visible: noteObj.notePosY > 0
      x: head.x - width - 0.1
  }

  Rectangle {
      id: stem
      width: 0.3
      height: 6
      color: activPal.text
      visible: noteObj.stemVisible()
  }

  Text {
      id: flag
      font { family: "Scorek"; pixelSize: 7 }
      color: activPal.text
      x: stem.x + 0.1
      y: stem.y - 15
  }

  Repeater { // upper lines
      model: 7
      Rectangle {
        x: head.x - 0.5
        y: 2 * (index + 1) - 0.1
        height: 0.2
        width: 3.5
        color: noteSegment.color
        visible: noteObj.notePosY > 0 && index >= Math.floor((noteObj.notePosY - 1) / 2)
      }
  }
  Repeater { // lower lines
      model: (staff.height - noteObj.upperLinePos() - 12) / 2
      Rectangle {
        x: head.x - 0.5
        y: noteObj.upperLinePos() + 10.0 + 2 * index - 0.1
        height: 0.2
        width: 3.5
        color: noteSegment.color
        visible: noteObj.upperLinePos() + 10 + index * 2 <= noteObj.notePosY
      }
  }

  function update() {
    alter.text = noteObj.alter()
    head.text = noteObj.head()
    stem.visible = noteObj.stemVisible()
    if (noteObj.stemVisible()) {
      stem.x = noteObj.stemDown() ? head.x : head.x + 2.0
      stem.y = noteObj.notePosY - (noteObj.stemDown() ? 0 : stem.height)
      flag.y = noteObj.stemDown() ? stem.y - stem.height - 3 : stem.y - 15
    }
    flag.text = noteObj.flag()
  }
}
