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

  property alias fontSize: nootkaWindow.font.pixelSize

  SystemPalette { id: activPal; colorGroup: SystemPalette.Active }
  SystemPalette { id: disdPal; colorGroup: SystemPalette.Disabled }

  width: GLOB.geometry.width
  height: GLOB.geometry.height
  x: GLOB.geometry.x
  y: GLOB.geometry.y

  onClosing: GLOB.geometry = Qt.rect(x ,y, width, height)

  header: TtoolBar {
      onAbout: {
        var c = Qt.createComponent("qrc:/TaboutNootka.qml")
        var a = c.createObject(nootkaWindow)
      }
      onSettings: {
        var c = Qt.createComponent("qrc:/TsettingsDialog.qml")
        var a = c.createObject(nootkaWindow)
      }
      onExam: {
        randNotes()
      }
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
        Component.onCompleted: {
          for (var n = 1; n < 8; ++n) {
            addNote(Noo.note(1 + Math.random() * 7, -2 + Math.random() * 5, Math.min(Math.max(-2, -3 + Math.random() * 6), 2)),
                             1 + Math.random() * 4)
          }
        }
      }

      // space for an instrument
      Rectangle { height: nootkaWindow.height / 4; Layout.fillWidth: true; color: activPal.window; border { width: 1; color: activPal.text } }
  }

//   Timer {
//       interval: 1000
//       running: true
//       repeat: true
//       onTriggered: {
//         var noteNr = Math.random() * 7
//         var rest = Math.floor((Math.random() * 100) % 2)
//         var accid = rest ? 0 : Math.min(Math.max(-2, -3 + Math.random() * 6), 2)
//         var note = Noo.note(1 + Math.random() * 7, -3 + Math.random() * 7, accid, 1 + Math.random() * 5, rest)
//         score.setNote(0, noteNr, note)
//         score.enableKeySign = !score.enableKeySign
//         if (score.enableKeySign)
//           score.setKeySignature(-7 + Math.random() * 15)
//         var m = Math.pow(2, Math.floor(1 + Math.random() * 11))
//         console.log("meter " + m)
//         score.meter = m
//       }
//   }
  function randNotes() {
    var noteNr = Math.random() * 7
    var rest = Math.floor((Math.random() * 100) % 2)
    var accid = rest ? 0 : Math.min(Math.max(-2, -3 + Math.random() * 6), 2)
    var note = Noo.note(1 + Math.random() * 7, -3 + Math.random() * 7, accid, 1 + Math.random() * 5, rest)
    score.setNote(0, noteNr, note)
  }

}
