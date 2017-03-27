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

import Nootka 1.0


Button {
  id: menuButton

  property Taction action

  width: parent.width - 6
  height: nootkaWindow.fontSize * 3
  anchors.horizontalCenter: parent.horizontalCenter

  onActionChanged: {
    icon.source = action.icon
    menuButton.text = action.text
    if (action.checkable)
      radioComp.createObject(contentItem)
  }

  contentItem: Item {
      width: menuButton.width
      Image {
        x: (nootkaWindow.fontSize * 2.5 - width) / 2
        id: icon
        sourceSize.height: nootkaWindow.fontSize * 2
        anchors.verticalCenter: parent.verticalCenter
      }
      Column {
        x: nootkaWindow.fontSize * 3
        anchors.verticalCenter: parent.verticalCenter
        Text {
          id: butText
          text: menuButton.text
          font.bold: true
        }
      }
  }

  Component {
    id: radioComp
    CheckBox {
      anchors {verticalCenter: parent.verticalCenter}
      checked: action.checked
      onClicked: menuButton.clicked()
    }
  }

  Component {
    id: shortComp
    Text {
      anchors {verticalCenter: parent.verticalCenter}
      text: action.key()
      x: menuButton.width - width - nootkaWindow.fontSize
    }
  }

  Component.onCompleted: { // shortcut is known only now
    if (!Noo.isAndroid() && action.shortcut)
      shortComp.createObject(contentItem)
  }

  onClicked: {
    if (action) {
      if (action.checkable)
        action.checked = !action.checked
      action.trigger()
    }
  }
}
