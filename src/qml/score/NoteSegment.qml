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


Item {
  id: noteSegment

  height: parent.height
  width: 7

  property real notePos: 0
  property color color: activPal.text

  Text {
      id: noteHead
      font { family: "Scorek"; pixelSize: 7 }
      y: notePos - staff.upperLine
      text: "\ue1d5"
      color: noteSegment.color
      visible: notePos > 0
  }

  Repeater { // upper lines
      id: upRep
      model: 7
      Rectangle {
        x: noteHead.x - 0.5
        y: 2 * (index + 1) - 0.1
        height: 0.18
        width: 3.5
        color: noteSegment.color
        visible: notePos > 0 && (index) >= Math.floor(notePos / 2) - 1
      }
  }
  Repeater { // upper lines
      id: lowRep
      model: (staff.height - staff.upperLine - 12) / 2
      Rectangle {
        x: noteHead.x - 0.5
        y: staff.upperLine + 9.9 + 2 * index
        height: 0.18
        width: 3.5
        color: noteSegment.color
        visible: staff.upperLine + 10 + index * 2 <= notePos - 1
      }
  }
}
