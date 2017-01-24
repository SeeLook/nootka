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
import QtQuick.Layouts 1.3
import QtQuick.Window 2.0


ApplicationWindow {
  id: nootkaWindow
  visible: true
  title: "Nootka"
  color: activPal.window

  SystemPalette { id: activPal; colorGroup: SystemPalette.Active }
  SystemPalette { id: disdPal; colorGroup: SystemPalette.Inactive }

  width: GLOB.geometry.width
  height: GLOB.geometry.height
  x: GLOB.geometry.x
  y: GLOB.geometry.y

//   onClosing: GLOB.geometry = Qt.rect(x ,y, width, height) TODO: enable it when time will come

  header: TtoolBar {
      onAbout: {
        var c = Qt.createComponent("qrc:/TaboutNootka.qml")
        var a = c.createObject(nootkaWindow)
      }
      onSettings: {}
  }

  ColumnLayout {
      anchors.fill: parent

      Row {
        height: nootkaWindow.height / 12
        width: nootkaWindow.width
        Label {
          id: statLab
          text: "Bla bla bla"
          verticalAlignment: Qt.AlignVCenter
          horizontalAlignment: Qt.AlignHCenter
          height: parent.height
          width: parent.width * 0.6
          color: activPal.text
        }

        PitchView {
          id: pitchView
          height: parent.height
          width: parent.width * 0.4
        }
      }

      Score {
        id: score
        Layout.fillWidth: true
        Layout.fillHeight: true
      }

      Rectangle { height: nootkaWindow.height / 4; Layout.fillWidth: true; color: activPal.window; border { width: 1; color: activPal.text } }
  }

//   Timer {
//       interval: 2000
//       running: true
//       repeat: true
//       onTriggered: {
//         score.enableKeySign = !score.enableKeySign
//         if (score.enableKeySign)
//           score.setKeySignature(-7 + Math.random() * 15)
//         var randNote3 = nRep.itemAt(Math.random() * nRep.model)
//         randNote3.pitch = 1 + Math.random() * 7
//         randNote3.octave = -1 + Math.random() * 4
//         randNote3.alter = Math.min(Math.max(-1, -2 + Math.random() * 5), 1)
//         key.key = -7 + Math.random() * 15
//       }
//   }

}
