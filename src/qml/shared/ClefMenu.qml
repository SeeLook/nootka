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

Grid {
  id: clefMenu
  columns: 1
  width: parent.width
  height: childrenRect.height
  spacing: nootkaWindow.fontSize / 4
  horizontalItemAlignment: Grid.AlignHCenter

  property var clefNr: [0, 1, 2, 4, 8, 32, 128]
  property var clefOff: [1.5, 0.8, 2.2, 1.5 , 0.8, 2.25, 0.8]
  property int selClef
  
  signal clicked(var cl)

  onSelClefChanged: {
    for (var c = 0; c < 7; ++c) {
      clefButtons.itemAt(c).highlighted = (clefNr[c] === selClef)
    }
  }

  Component {
    id: clefPixComp
    Item {
      property int index: 0
      Text {
        id: clefStaff
        x: scale * 1.5
        font { family: "scorek"; pixelSize: 3 }
        text: "\ue014\ue014"
        color: activPal.text
      }
      Text {
        x: clefStaff.x + 0.3 * scale
        y: clefStaff.y - clefOff[index] * scale
        font { family: "scorek"; pixelSize: 3 }
        text: Noo.clef(clefNr[index]).glyph()
        color: activPal.text
      }
    }
  }

  Repeater {
    id: clefButtons
    model: 7
    Button {
      width: (clefMenu.width * 0.98) / clefMenu.columns
      height: nootkaWindow.fontSize * (index === 6 ? 11.0 : 7.5)
      onClicked: {
        selClef = clefNr[index]
        clefMenu.clicked(clefNr[index])
      }
      Row {
        height: parent.height
        width: parent.width
        spacing: nootkaWindow.fontSize
        Item { id: clefItem; width: nootkaWindow.fontSize * 5.0; height: nootkaWindow.fontSize }
        Column {
          anchors.verticalCenter: parent.verticalCenter
          width: parent.width - nootkaWindow.fontSize * 6.0
          spacing: nootkaWindow.fontSize / 4
          Text {
            antialiasing: true
            text: Noo.clef(clefNr[index]).name()
            font { bold: true; pixelSize: nootkaWindow.fontSize * 1.1 }
            color: activPal.text
          }
          Text {
            antialiasing: true
            text: Noo.clef(clefNr[index]).desc()
            font { pixelSize: nootkaWindow.fontSize * 0.8 }
            width: parent.width
            wrapMode: Text.WordWrap
            color: activPal.text
          }
        }
        Component.onCompleted:{
          clefPixComp.createObject(clefItem, {"scale": nootkaWindow.fontSize, "y":  - 2.0 * nootkaWindow.fontSize, "index": index})
          if (index == 6) {
            var lowStaff = clefPixComp.createObject(clefItem, {"index": 2, "scale": nootkaWindow.fontSize, "y": nootkaWindow.fontSize * 2.9})
            var brace = Qt.createQmlObject('import QtQuick 2.7; Text { font {family: "scorek"; pixelSize: 8 } text: "\ue000"; x: 0.85 }', lowStaff)
            brace.y = -10.0
            brace.color = activPal.text
          }
        }
      }
    }
  }
}
