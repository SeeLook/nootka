/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


TipRect {
  id: tip

  property alias text: text.text
  property color bg: activPal.window
  property real offX: 0
  property real offY: 0

  // private
  property real eW: 0
  property real eH: 0

  color: Qt.tint(activPal.window, Noo.alpha(bg, 50))
  shadowRadius: Noo.fontSize()
  z: 200; x: offX - width / 2; y: offY - height / 2
  width: text.width; height: text.height
  scale: GLOB.useAnimations ? 0.1 : 1.0

  MouseArea {
    id: dragArea
    anchors.fill: parent
    drag.target: parent
    drag.minimumX: 1; drag.maximumX: executor.width - width * scale - 1
    drag.minimumY: 1; drag.maximumY: executor.height - height * scale - 1
  }

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

  Drag.active: dragArea.drag.active

  Component.onCompleted: {
    eW = executor.width; eH = executor.height
    if (GLOB.useAnimations)
      scale = 1.0
  }

  Connections {
    target: executor
    onWidthChanged: {
      scale = (executor.width / eW) * scale
      x = Math.min(x * (executor.width / eW), executor.width - tip.width * scale - 1)
      eW = executor.width
    }
    onHeightChanged: {
      y = Math.min(y *(executor.height / eH), executor.height - tip.height * scale - 1)
      eH = executor.height
    }
    onDestroyTips: tip.destroy()
  }

  Behavior on scale { SpringAnimation { spring: 2; damping: 0.2; epsilon: 0.005; duration: 300 }}
}
