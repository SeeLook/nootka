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
import QtQuick.Window 2.0


Item {

  property alias description: descText.text
  property alias descriptionColor: descText.color
  default property alias content: container.data

  width: parent.width
  height: descText.height + line.height + container.height + nootkaWindow.font.pixelSize

//   Rectangle { anchors.fill: parent }

  Column {
    id: col
    spacing: nootkaWindow.font.pixelSize / 2
    width: parent.width

    Item {
      id: container
      width: childrenRect.width
      height: childrenRect.height
      anchors.horizontalCenter: parent.horizontalCenter
    }

    Text {
      id: descText
      anchors { horizontalCenter: parent.horizontalCenter; bottom: line.Top }
      font.pixelSize: nootkaWindow.font.pixelSize * 0.9
      textFormat: Text.RichText
      horizontalAlignment: Text.AlignHCenter
    }

    Rectangle {
      id: line
      anchors.bottom: parent.Bottom
      width: parent.width
      height: nootkaWindow.font.pixelSize / 6
      color: activPal.text
    }
  }
}
