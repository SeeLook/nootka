/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

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
