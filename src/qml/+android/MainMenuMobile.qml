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
import QtGraphicalEffects 1.0
import QtQuick.Window 2.0


Item {
  property Item toolBar: null // fake, for main window information

  x: Screen.pixelDensity / 2
  y: Screen.pixelDensity / 2
  z: 100
  width: nootkaWindow.fontSize * 1.5
  height: nootkaWindow.fontSize * 4

  Rectangle {
    id: bg
    anchors.fill: parent
    color: area.pressed ? activPal.highlight : Qt.rgba(0.2, 0.2, 0.2, 0.05)
    radius: nootkaWindow.fontSize / 3
    visible: false
  }
  DropShadow {
    anchors.fill: bg
    horizontalOffset: nootkaWindow.fontSize / 5
    verticalOffset: nootkaWindow.fontSize / 5
    radius: 8.0
    samples: 17
    color: activPal.shadow
    source: bg
  }

  Repeater {
    model: 3
    Rectangle {
      width: nootkaWindow.fontSize * 0.5
      height: nootkaWindow.fontSize * 0.5
      x: nootkaWindow.fontSize * 0.5
      y: nootkaWindow.fontSize * 0.625 * (index + 1) + index * nootkaWindow.fontSize * 0.5
      color: "black"
      radius: width / 2
    }
  }

  Drawer {
    id: mainDrawer
    width: nootkaWindow.fontSize * 16
    height: nootkaWindow.height
    onVisibleChanged: {
      nooLabel.bgColor = visible ? nooLabel.randColor() : "white"
    }
    Flickable {
      anchors.fill: parent
      clip: true
      contentHeight: childrenRect.height
      Column {
        width: parent.width
        spacing: Screen.pixelDensity
        NootkaLabel {
          id: nooLabel
          height: nootkaWindow.fontSize * 6.328125 // 0.3955078125 * 16
          onClicked: {
            mainDrawer.close()
            nootkaWindow.aboutAct.trigger()
          }
        }
        MenuButton { action: nootkaWindow.levelAct; onClicked: mainDrawer.close() }
        MenuButton { action: nootkaWindow.examAct; /*onClicked: mainDrawer.close()*/ }
        MenuButton { action: nootkaWindow.settingsAct; onClicked: mainDrawer.close() }
      }
    }
  }

  MouseArea {
    id: area
    anchors.fill: parent
    onClicked: mainDrawer.open()
  }
}
