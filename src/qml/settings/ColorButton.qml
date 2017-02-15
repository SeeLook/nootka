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
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0


Item {
  id: colorButton
  property color color: "white"
  property real offset: Screen.pixelDensity / 2

  implicitWidth: nootkaWindow.fontSize * 4
  implicitHeight: nootkaWindow.fontSize * 2

  Rectangle {
    id: colorRect
    anchors.fill: parent
    radius: height / 2
    color: enabled ? colorButton.color : disdPal.text
    visible: false
  }

   DropShadow {
    id: shadow
    anchors.fill: colorRect
    horizontalOffset: offset
    verticalOffset: offset
    radius: Screen.pixelDensity
    samples: 17
    color: activPal.shadow
    source: colorRect
  }

  MouseArea {
    anchors.fill: parent
    onPressed: offset = 0
    onReleased: offset = Screen.pixelDensity / 2
    onClicked: colorDialog.open()
  }

  ColorDialog {
    id: colorDialog
    color: colorButton.color
    onAccepted: colorButton.color = colorDialog.color
    modality: Qt.WindowModal
    showAlphaChannel: false
  }
}
