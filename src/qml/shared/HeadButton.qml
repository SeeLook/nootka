/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import Nootka 1.0


ToolButton {
  antialiasing: true
  hoverEnabled: true

  implicitWidth: Math.max(pix.width, butText.width) + (Noo.isAndroid() ? 4 : factor * 2)
  implicitHeight: butText.height + pix.height + Screen.pixelDensity * 2

  property alias pixmap: pix.source
  property alias name: butText.text
  property alias tip: toolTip.text
  property real factor: nootkaWindow.height / 140
  property alias fontSize: butText.font.pixelSize
  property alias textColor: butText.color
  property Taction taction

  background: Rectangle { color: pressed ? activPal.button : (hovered ? activPal.base : "transparent") }

  onTactionChanged: {
    if (taction) {
      pix.source = taction.icon
      name = taction.text
      tip = taction.tip
    }
  }

  onClicked: {
    if (taction)
      taction.trigger()
    focus = false
  }

  Image {
    id: pix
    y: Screen.pixelDensity
    sourceSize.height: factor * 8
    anchors.horizontalCenter: butText.horizontalCenter
  }
  Text {
    id: butText
    font.pixelSize: Math.min(factor * 2.5, Noo.fontSize())
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: pix.bottom
    horizontalAlignment: Text.AlignHCenter
    color: activPal.text
  }

  ToolTip {
    id: toolTip
    delay: 1000
    timeout: 5000
    visible: hovered && text !== ""
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
