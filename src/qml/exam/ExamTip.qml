/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Window 2.2

import "../"


TipRect {
  id: tip

  property alias text: text.text
  property alias textItem: text
  property alias font: text.font
  property color bg: activPal.base
  property real offX: 0
  property real offY: 0
  property alias showExit: exitImg.visible

  // private
  property real eW: 0
  property real eH: 0

  color: Qt.tint(activPal.base, Noo.alpha(bg, 50))
  border { width: Noo.fontSize() / 6; color: bg }
  shadowRadius: Noo.fontSize() * (Noo.isAndroid() ? 1 : 3)
  horizontalOffset: Noo.isAndroid() ? 0 : Noo.fontSize() / 3; verticalOffset: horizontalOffset
  z: 200
  x: Noo.bound(shadowRadius, offX - width / 2, executor.width - width - shadowRadius)
  y: Noo.bound(shadowRadius, offY - height / 2, executor.height - height - shadowRadius)
  width: text.width; height: text.height
  scale: GLOB.useAnimations ? 0 : 1

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

  Image {
    id: exitImg
    visible: false
    anchors { right: parent.right; top: parent.top; margins: tip.width / 80 }
    source: Noo.pix("exit")
    sourceSize.width: tip.width / 20
    MouseArea {
      anchors.fill: parent
      onClicked: tip.visible = false
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
  }

  Behavior on scale { SpringAnimation { spring: 2; damping: 0.2; epsilon: 0.005; duration: 300 }}
}
