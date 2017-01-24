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


Text {
  id: clef

  property int type: Tclef.Treble_G
  property bool readOnly: false

  x: 0.5
  y: 5
  text: "\ue050"
  font { family: "Scorek"; pixelSize: 8 }
  color: activPal.text

  Drawer {
    id: clefDrawer
    width: nootkaWindow.width / 4
    height: nootkaWindow.height
    Column {
      width: parent.width
      spacing: 10
      Button { text: Noo.clef(Tclef.Treble_G).name(); width: parent.width; onClicked: clefClicked(Tclef.Treble_G)  }
      Button { text: Noo.clef(Tclef.Treble_G_8down).name(); width: parent.width; onClicked: clefClicked(Tclef.Treble_G_8down) }
      Button { text: Noo.clef(Tclef.Bass_F).name(); width: parent.width; onClicked: clefClicked(Tclef.Bass_F) }
      Button { text: Noo.clef(Tclef.Bass_F_8down).name(); width: parent.width; onClicked: clefClicked(Tclef.Bass_F_8down) }
      Button { text: Noo.clef(Tclef.Alto_C).name(); width: parent.width; onClicked: clefClicked(Tclef.Alto_C) }
      Button { text: Noo.clef(Tclef.Tenor_C).name(); width: parent.width; onClicked: clefClicked(Tclef.Tenor_C) }
      Button { text: Noo.clef(Tclef.PianoStaffClefs).name(); width: parent.width; onClicked: clefClicked(Tclef.PianoStaffClefs) }
    }
  }

  MouseArea {
    anchors.fill: parent
    enabled: !readOnly
    onClicked: {
      clefDrawer.open()
    }
  }

  onTypeChanged: {
      if (clef.type === Tclef.PianoStaffClefs) {
        console.log("Piano clef not supported yet")
        return
      }
      switch (clef.type) {
        case Tclef.Treble_G:
          clef.y = 5; text = "\ue050"
          break;
        case Tclef.Treble_G_8down:
          clef.y = 5; text = "\ue052"
          break;
        case Tclef.Bass_F:
          clef.y = 1; text = "\ue062"
          break;
        case Tclef.Bass_F_8down:
          clef.y = 1; text = "\ue064"
          break;
        case Tclef.Alto_C:
          clef.y = 3; text = "\ue05c"
          break;
        case Tclef.Tenor_C:
          clef.y = 1; text = "\ue05c"
          break;
      }
  }

  function clefClicked(c) {
    clef.type = c
    clefDrawer.close()
  }
}




