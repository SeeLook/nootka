/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import Nootka 1.0
import "../"


ToolButton {
  id: root

  hoverEnabled: true

  width: pix.width + (Noo.isAndroid() ? 4 : factor * 2)
  height: pix.height + Screen.pixelDensity * 2

  property alias pixmap: pix.source
  property real factor: analyzeWindow.height / 150
  property Taction taction

  background: Rectangle {
    color: Noo.alpha(activPal.highlight, 50)
    scale: hovered ? 1 : 0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  }

  onTactionChanged: {
    if (taction) {
      pix.source = taction.icon
      toolTip.text = taction.text
    }
  }

  onClicked: {
    if (taction)
      taction.trigger()
    focus = false
  }

  Image {
    id: pix
    y: Screen.pixelDensity; x: Noo.isAndroid() ? 2 : factor
    sourceSize.height: factor * 10
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
    enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1 }}
    exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0 }}
    background: TipRect { shadowRadius: Noo.fontSize(); color: activPal.highlight }
  }
}
