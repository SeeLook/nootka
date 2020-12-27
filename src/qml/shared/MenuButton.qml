/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0


Rectangle {
  id: menuButton

  property Taction action
  property alias text: butText.text
  property alias iconHeight: icon.height
  property alias containsMouse: ma.containsMouse
  property alias containsPress: ma.containsPress
  property alias textColor: butText.color

  width: parent.width - Noo.fontSize()
  implicitHeight: Noo.fontSize() * 2.8
  anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
  color: ma.containsPress ? activPal.highlight : (ma.containsMouse ? Noo.alpha(activPal.highlight, 50) : "transparent")
  enabled: !action || action.enabled

  signal clicked()

  // private
  property var shortText: null

  scale: GLOB.useAnimations && ma.pressed ? 0.9 : 1.0
  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}

  onActionChanged: {
    if (action && action.checkable)
      radioComp.createObject(menuButton)
  }

  Image {
    id: icon
    source: action && !action.checkable ? action.icon : ""
    x: Noo.fontSize() / 2
    height: Noo.fontSize() * 2.2; width: height * (sourceSize.width / sourceSize.height)
    anchors.verticalCenter: parent.verticalCenter
    scale: GLOB.useAnimations && !ma.pressed && ma.containsMouse ? 1.2 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    visible: menuButton.enabled
  }
  Text {
    id: butText
    x: Noo.fontSize() * (action && (action.icon !== "" || action.checkable) ? 3.7 : 0.8)
    text: action ? action.text : ""
    font { bold: true; pixelSize: Noo.fontSize() }
    color: enabled ? activPal.text : disdPal.text
    width: parent.width - x - Noo.fontSize() - (shortText ? shortText.width : 0)
    height: parent.height
    verticalAlignment: Text.AlignVCenter
    fontSizeMode: Text.HorizontalFit; minimumPixelSize: Noo.fontSize() / 2; minimumPointSize: minimumPixelSize
    elide: Text.ElideRight
  }

  Component {
    id: radioComp
    TcheckBox {
      anchors.verticalCenter: parent.verticalCenter
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
      color: enabled ? activPal.text : disdPal.text
    }
  }

  Component.onCompleted: { // shortcut is known only now
    if (!Noo.isAndroid() && action && action.shortcut)
      shortText = shortComp.createObject(menuButton)
  }

  MouseArea {
    id: ma
    anchors.fill: parent
    hoverEnabled: true
    onClicked: buttonClicked()
    onHoveredChanged: {
      if (action) {
        if (GLOB.showHints && action.tip !== "" && ma.containsMouse)
          Noo.setStatusTip(action.tip, action.tipPos)
        else
          Noo.setStatusTip("", action.tipPos)
      }
    }
  }

  function buttonClicked() {
    menuButton.clicked()
    if (action)
      action.trigger()
  }
}
