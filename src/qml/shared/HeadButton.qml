/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import Nootka 1.0


ToolButton {
  antialiasing: true
  hoverEnabled: true

  implicitWidth: Math.max(icon.width, butText.width) + (Noo.isAndroid() ? 4 : factor * 2)
  implicitHeight: butText.height + icon.height + Screen.pixelDensity * 2

  property alias icon: icon.source
  property alias name: butText.text
  property alias tip: toolTip.text
  property real factor: nootkaWindow.height / 140
  property alias fontSize: butText.font.pixelSize
  property alias textColor: butText.color
  property Taction action

  onActionChanged: {
    if (action) {
      icon.source = action.icon
      name = action.text
      tip = action.tip
    }
  }

  onClicked: {
    if (action)
      action.trigger()
    focus = false
  }

  Image {
    id: icon
    y: Screen.pixelDensity
    sourceSize.height: factor * 8
    anchors.horizontalCenter: butText.horizontalCenter
  }
  Text {
    id: butText
    font.pixelSize: Math.min(factor * 2.5, nootkaWindow.fontSize)
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: icon.bottom
    horizontalAlignment: Text.AlignHCenter
    color: activPal.text
  }

  ToolTip {
    id: toolTip
    delay: 1000
    timeout: 5000
    visible: hovered && text != ""
    contentItem: Text {
      text: toolTip.text
      color: activPal.highlightedText
    }
    background: Rectangle {
      border.color: activPal.highlightedText
      color: activPal.highlight
    }
  }
}
