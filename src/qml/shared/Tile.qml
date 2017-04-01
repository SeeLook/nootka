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


Frame {
  property alias description: descText.text
  property alias descriptionColor: descText.color
  default property alias content: container.data

  background: Rectangle {
    id: bg
    color: enabled ? Qt.lighter(activPal.window, 1.05) : Qt.darker(disdPal.window, 1.5)
    border.color: disdPal.text
    radius: nootkaWindow.fontSize / 4
  }

  width: parent.width * 0.98
  height: (descText.text === "" ? 0 : descText.height) + container.height + nootkaWindow.fontSize * 2

  Column {
      id: col
      spacing: nootkaWindow.fontSize / 2
      width: parent.width

      Item {
        id: container
        width: parent.width - nootkaWindow.fontSize
        height: childrenRect.height
      }

      Text {
        id: descText
        anchors.bottom: parent.Bottom
        width: parent.width
        font.pixelSize: nootkaWindow.fontSize * 0.8
        textFormat: Text.RichText
        horizontalAlignment: Text.AlignHCenter
        color: enabled ? activPal.text : disdPal.text
        wrapMode: Text.WordWrap
      }

  }
}
