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

import Score 1.0

ApplicationWindow {
  id: nootkaWindow
  visible: true
  title: "Nootka"

  width: {
    var r = GLOB.getVar("General/geometry")
    return (r && r.width >= 720) ? r.width : Math.max(Screen.desktopAvailableWidth * 0.75, 720)
  }
  height: {
    var r = GLOB.getVar("General/geometry")
    return (r && r.height >= 480) ? r.height : Math.max(Screen.desktopAvailableHeight * 0.75, 480)
  }
  x: {
    var r = GLOB.getVar("General/geometry")
    return (r && r.x >= 0) ? r.x : (Screen.desktopAvailableWidth - nootkaWindow.width) / 2
  }
  y: {
    var r = GLOB.getVar("General/geometry")
    return (r && r.y >= 0) ? r.y : (Screen.desktopAvailableHeight - nootkaWindow.height) / 2
  }

  header: TtoolBar {}

  ColumnLayout {
    anchors.fill: parent
    Score {
      Layout.fillWidth: true
      Layout.fillHeight: true

      Note {
        pitch: 6
        octave: 1
        alter: -1
        rhythm: Note.Esixteenth
      }
      Note {
        pitch: 5
        octave: 0
        alter: 1
        rhythm: Note.Esixteenth
      }
    }

    Rectangle { height: nootkaWindow.height / 3; Layout.fillWidth: true; color: "blue" }
  }

  Component.onCompleted: {}

}
