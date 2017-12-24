/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0


Rectangle {
  id: menuButton

  property Taction action
  property alias text: butText.text

  width: parent.width - Noo.fontSize()
  implicitHeight: Noo.fontSize() * 3
  anchors.horizontalCenter: parent.horizontalCenter
  color: ma.containsPress ? activPal.highlight : activPal.button
  border { width: 1; color: ma.containsMouse ? activPal.highlight : activPal.shadow }
  radius: Noo.fontSize() / 4

  signal clicked()

  onActionChanged: {
    if (action.checkable)
      radioComp.createObject(menuButton)
  }

  Image {
    x: Noo.fontSize() / 2
    id: icon
    sourceSize.height: Noo.fontSize() * 2.2
    anchors.verticalCenter: parent.verticalCenter
    source: action ? action.icon : ""
  }
  Column {
    x: Noo.fontSize() * 3.7
    anchors.verticalCenter: parent.verticalCenter
    Text {
      id: butText
      text: action ? action.text : ""
      font.bold: true
    }
  }

  Component {
    id: radioComp
    TcheckBox {
      anchors {verticalCenter: parent.verticalCenter}
      checked: menuButton.action.checked
      onClicked: menuButton.clicked()
      x: (Noo.fontSize() * 3.5 - width) / 2
    }
  }

  Component {
    id: shortComp
    Text {
      anchors {verticalCenter: parent.verticalCenter}
      text: "(" + action.key() + ")"
      x: menuButton.width - width - Noo.fontSize()
    }
  }

  Component.onCompleted: { // shortcut is known only now
    if (!Noo.isAndroid() && action.shortcut)
      shortComp.createObject(menuButton)
  }

  MouseArea {
    id: ma
    anchors.fill: parent
    hoverEnabled: true
    onClicked: {
      menuButton.clicked()
      if (action) {
        if (action.checkable)
          action.checked = !action.checked
        action.trigger()
      }
    }
  }
}
