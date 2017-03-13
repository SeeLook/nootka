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

import Score 1.0


Item {
  id: noteCursor

  anchors.fill: parent

  property color color: activPal.highlight
  property real yPos: 0
  property real upperLine: 16

  signal clicked(var y)

  visible: yPos > 0

  Text {
      id: head
      scale: 1.2
      font { family: "Scorek"; pixelSize: 7 }
      text: "\uf4be"
      y: yPos - 15
      color: noteCursor.color

      Text {
        id: alter
        font { family: "Scorek"; pixelSize: 7 }
        color: noteCursor.color
        x: head.x - width - 0.1
      }
  }

  Repeater { // upper lines
      model: (upperLine - 2) / 2
      Rectangle {
        x: - 1.0
        y: 2 * (index + 1) - 0.1
        height: 0.2
        width: 4.0
        color: noteCursor.color
        visible: yPos > 0 && index >= Math.floor((yPos - 1) / 2)
      }
  }
  Repeater { // lower lines
      model: (parent.height - upperLine - 12) / 2
      Rectangle {
        x: - 1.0
        y: upperLine + 10.0 + 2 * index - 0.1
        height: 0.2
        width: 4.0
        color: noteCursor.color
        visible: upperLine + 10.0 + index * 2 <= yPos
      }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: noteCursor.clicked(yPos)
  }

}
