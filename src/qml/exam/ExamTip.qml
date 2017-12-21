/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Window 2.2

import "../"


TipRect {
  id: tip

  property alias text: text.text
  property alias font: text.font
  property color bg: activPal.window
  property real offX: 0
  property real offY: 0

  // private
  property real eW: 0
  property real eH: 0

  color: Qt.tint(activPal.window, Noo.alpha(bg, 50))
  shadowRadius: Screen.height / 90
  z: 200
  x: Noo.bound(shadowRadius, offX - width / 2, executor.width - width - shadowRadius)
  y: Noo.bound(shadowRadius, offY - height / 2, executor.height - height - shadowRadius)
  width: text.width; height: text.height
  scale: GLOB.useAnimations ? 0.1 : 1.0

  Text {
    id: text
    color: activPal.text
    textFormat: Text.RichText
    padding: Noo.fontSize()
    onLinkActivated: {
      executor.tipLink(link)
      tip.destroy()
    }
    MouseArea {
      anchors.fill: parent
      acceptedButtons: Qt.NoButton
      cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
    }
  }

  Component.onCompleted: {
    eW = executor.width; eH = executor.height
    if (GLOB.useAnimations)
      scale = 1.0
  }

  Connections {
    target: executor
    onWidthChanged: {
      scale = (executor.width / eW) * scale
      offX = offX * (executor.width / eW)
      eW = executor.width
    }
    onHeightChanged: {
      offY = offY *(executor.height / eH)
      eH = executor.height
    }
    onDestroyTips: tip.destroy()
  }

  Behavior on scale { SpringAnimation { spring: 2; damping: 0.2; epsilon: 0.005; duration: 300 }}
}
