/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

import Score 1.0
import Note 1.0


ApplicationWindow {
  id: scorekWindow
  visible: true
  title: "Scorek QML test"
  width: Screen.desktopAvailableWidth / 2
  height: Screen.desktopAvailableHeight / 2

  ColumnLayout {
    anchors.fill: parent
    Flickable {
      Layout.fillHeight: true
      Layout.fillWidth: true
      contentWidth: scorekWindow.width; contentHeight: lowest.y + lowest.height
      ScrollBar.vertical: ScrollBar {}

      Score {
        id: upper
        anchors.top: parent.top
        height: scorekWindow.height / 2
        width: scorekWindow.width

        clef: Score.Etreble_G_8down

        Note {
          id: firstNote
          pitch: 5
          octave: 1
        }
        Note {
          pitch: 6
          octave: 2
        }

        Repeater {
          model: 1

          Repeater {
            model: 16

            Note {
              pitch: 1 + index % 7
              octave: Math.floor(index / 7)
              rhythm: Note.Esixteenth
            }
          }
        }
      }
      Score {
        id: lower
        y: upper.height * 0.7
        height: scorekWindow.height / 2
        width: scorekWindow.width

        clef: Score.Etreble_G

        Note {
          id: note2
          pitch: 6
          octave: 1
        }
      }
      Score {
        id: lowest
        y: lower.y + lower.height * 0.7
        height: scorekWindow.height / 2
        width: scorekWindow.width

        clef: Score.Etreble_G

        Note {
          id: note3
          pitch: 4
          octave: 1
        }
        Repeater {
            id: lowRep
            model: 14

            Note {
              pitch: 1 + Math.random() * 7
              octave: Math.floor(index / 7)
              alter: -1 + Math.random() * 3
              rhythm: Note.Eeighth
            }
          }
      }
    }

    Rectangle {
      height: scorekWindow.height / 3 ; Layout.fillWidth: true; color: "lightGray" 
      Text {
        color: "orange"
        text: "Any instrument here"
        font.bold: true
        font.pixelSize: scorekWindow.font.pixelSize * 3
        anchors.centerIn: parent
      }
    }
    Timer {
      interval: 200
      running: true
      repeat: true
      onTriggered: {
        firstNote.pitch = 1 + Math.random() * 7
        note2.pitch = 1 + Math.random() * 7
        var randNote3 = lowRep.itemAt(Math.random() * lowRep.model)
        randNote3.pitch = 1 + Math.random() * 7
        randNote3.octave = -1 + Math.random() * 4
        randNote3.alter = Math.min(Math.max(-1, -2 + Math.random() * 5), 1)
      }
    }
  }

}
