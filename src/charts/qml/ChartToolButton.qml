/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
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
  enabled: (taction && taction.enabled) || !taction

  property alias pixmap: pix.source
  property real factor: analyzeWindow.height / 150
  property Taction taction

  background: Item {}

  onClicked: {
    if (taction)
      taction.trigger()
    focus = false
  }

  Image {
    id: pix
    mipmap: true
    anchors.centerIn: parent
    source: taction ? taction.icon : ""
    height: factor * 10; width: height * (sourceSize.width / sourceSize.height)
    transformOrigin: Image.Center
    scale: !enabled || pressed ? 0.6 : (hovered ? 1.1 : 0.8)
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  }

  ToolTip {
    id: toolTip
    delay: 750
    timeout: 5000
    visible: hovered && contentItem.text !== ""
    contentItem: Text {
      text: taction ? taction.text : ""
      color: activPal.highlightedText
    }
    enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1 }}
    exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0 }}
    background: TipRect { shadowRadius: Noo.fontSize(); color: activPal.highlight }
  }
}
