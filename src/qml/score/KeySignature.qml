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
  id: keySig

  property int key: 0
  property bool readOnly: false

  readonly property var sharpPos: [ 0, 3, -1, 2, 5, 1,  4 ]
  readonly property var flatPos: [ 4, 1, 5, 2, 6, 3, 7 ]
  property real accidOff: 0
  property int clef: Tclef.Treble_G

  width: Math.max(6, (Math.abs(key) + 1) * 1.8)
  height: 10

//   Rectangle { width: parent.width; height: 14; color: "#aaff0000"; y: 12 }

  function changeClef(c) {
      if (c === Tclef.Treble_G || c === Tclef.Treble_G_8down)
          accidOff = 1
      else if (c === Tclef.Bass_F || c === Tclef.Bass_F_8down)
          accidOff = -1
      else if (c === Tclef.Alto_C)
          accidOff = 0;
      else if (c === Tclef.Tenor_C)
          accidOff = 2
      else
          accidOff = 1
      clef = c
  }

  Repeater {
      model: 7
      Text {
        font { family: "Scorek"; pixelSize: 8 }
        color: activPal.text
        text: key < 0 ? "\ue260" : "\ue262"
        x: index * 1.8
        y: (key < 0 ? flatPos[index] : sharpPos[index]) - (accidOff) + (clef === Tclef.Tenor_C && key > 0 && (index === 0 || index === 2) ? 7 : 0)
        visible: index < Math.abs(key)
      }
  }

  MouseArea { // occupy only selected part of staff height
      width: parent.width; height: 14; y: 12
      enabled: !readOnly
      onClicked: {
        if (mouseY < 7) {
            if (key < 7)
              ++key
        } else {
          if (key > -7)
              --key
        }
      }
      onWheel: {
        if (wheel.angleDelta.y > 0) {
          if (key < 7)
            ++key
        } else if (wheel.angleDelta.y < 0) {
          if (key > -7)
            --key
        }
      }
  }
}
