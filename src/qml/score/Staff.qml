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
  id: staff

  property alias clef: clef

  property real linesCount: 40
  property int number: -1
  property real upperLine: 16.0

  height: linesCount
  width: score.width / scale
  scale: score.height / linesCount
  transformOrigin: Item.TopLeft

  Repeater { // staff lines
      model: 5
      Rectangle {
        x: 0.5
        y: upperLine + 2 * index - 0.1
        height: 0.18
        width: staff.width - 1.0
        color: activPal.text
      }
  }

  Clef {
    id: clef
    onTypeChanged: {
      // TODO: approve clef change to the notes
    }
  }

  Text { // staff number
      x: (clef.width - width) / 2
      y: clef.y + 4.5
      text: number + 1
      visible: number > -1
      font.pixelSize: 2
      color: activPal.text
  }

  Repeater {
      model: 8
      NoteSegment {
        notePos: upperLine + 11 - index
        x: 15 + index * width
      }
  }
}
