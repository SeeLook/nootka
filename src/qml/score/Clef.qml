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


Text {
  id: clef

  property int type: Tclef.Treble_G
  property bool readOnly: false
  property Drawer clefDrawer

  width: 5.5

  x: 0.5
  y: 5
  text: "\ue050"
  font { family: "Scorek"; pixelSize: 8 }
  color: activPal.text

  Component {
    id: clefComp
    Drawer {
      id: drawer
      property alias selectedClef: clefMenu.selClef
      width: nootkaWindow.fontSize * 20
      height: nootkaWindow.height
      visible: true
      Flickable {
        anchors.fill: parent
        clip: true
        contentHeight: clefMenu.height
        ClefMenu {
          id: clefMenu
          onClicked: { type = cl; close() }
        }
      }
    }
  }

  MouseArea {
    anchors.fill: parent
    enabled: !readOnly
    onClicked: {
      if (clefDrawer)
        clefDrawer.open()
      else
        clefDrawer = clefComp.createObject(clef)
      clefDrawer.selectedClef = type
    }
  }

  onTypeChanged: {
      x = 0.5
      switch (clef.type) {
        case Tclef.Treble_G:
        case Tclef.PianoStaffClefs:
        case Tclef.Treble_G_8down:
          clef.y = 5; break;
        case Tclef.Bass_F:
        case Tclef.Bass_F_8down:
        case Tclef.Tenor_C:
          clef.y = 1; break;
        case Tclef.Alto_C:
          clef.y = 3; break;
        case Tclef.NoClef:
          clef.y = 3; x = 2.0; break;
      }
      text = Noo.clef(clef.type).glyph()
  }
}




