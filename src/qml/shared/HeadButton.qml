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
import QtQuick.Window 2.0


ToolButton {
  antialiasing: true
  hoverEnabled: true

  implicitWidth: Math.max(icon.width, butText.width) + factor * 2
  implicitHeight: factor * 13 // 1.3 cm by default

  property alias icon: icon.source
  property alias name: butText.text
  property alias tip: toolTip.text
  property real factor: Screen.pixelDensity
  property alias color: bgRect.color
  property alias fontSize: butText.font.pixelSize

  Rectangle {
    id: bgRect
    anchors.fill: parent
    color: "transparent"
  }
  Image {
    id: icon
    y: Screen.pixelDensity
    sourceSize.height: factor * 8
    anchors.horizontalCenter: butText.horizontalCenter
  }
  Text {
    id: butText
    font.pixelSize: factor * 2.5
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: icon.bottom
  }

  ToolTip {
    id: toolTip
    delay: 1000
    timeout: 5000
    visible: hovered && text != ""
  }
}
