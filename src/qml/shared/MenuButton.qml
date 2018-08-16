/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0


Rectangle {
  id: menuButton

  property Taction action
  property alias text: butText.text

  width: parent.width - Noo.fontSize()
  implicitHeight: Noo.fontSize() * 2.8
  anchors.horizontalCenter: parent.horizontalCenter
  color: ma.containsPress ? activPal.highlight : (ma.containsMouse ? Noo.alpha(activPal.highlight, 50) : "transparent"  /*activPal.button*/)

  signal clicked()

  onActionChanged: {
    if (action && action.checkable)
      radioComp.createObject(menuButton)
  }

  Image {
    id: icon
    x: Noo.fontSize() / 2
    sourceSize.height: Noo.fontSize() * 2.2
    anchors.verticalCenter: parent.verticalCenter
    source: action ? action.icon : ""
  }
  Text {
    x: Noo.fontSize() * (action && (action.icon !== "" || action.checkable) ? 3.7 : 0.8)
    anchors.verticalCenter: parent.verticalCenter
    id: butText
    text: action ? action.text : ""
    font.bold: true
    color: activPal.text
    width: parent.width - x - Noo.fontSize() / 2; elide: Text.ElideRight
  }

  Component {
    id: radioComp
    TcheckBox {
      anchors {verticalCenter: parent.verticalCenter}
      checked: menuButton.action.checked
      onClicked: buttonClicked()
      x: (Noo.fontSize() * 3.5 - width) / 2
    }
  }

  Component {
    id: shortComp
    Text {
      anchors.verticalCenter: parent.verticalCenter
      text: action.key()
      font.pixelSize: Noo.fontSize() * 0.8
      x: menuButton.width - width - Noo.fontSize() / 2
      color: activPal.text
    }
  }

  Component.onCompleted: { // shortcut is known only now
    if (!Noo.isAndroid() && action && action.shortcut)
      shortComp.createObject(menuButton)
  }

  MouseArea {
    id: ma
    anchors.fill: parent
    hoverEnabled: true
    onClicked: buttonClicked()
    onHoveredChanged: {
      if (action.tip !== "" && ma.containsMouse)
        Noo.setStatusTip(action.tip, action.tipPos)
      else
        Noo.setStatusTip("", action.tipPos)
    }
  }

  function buttonClicked() {
    menuButton.clicked()
    if (action) {
      if (action.checkable)
        action.checked = !action.checked
      action.trigger()
    }
  }
}
