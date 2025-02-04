/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import Nootka 1.0
import "../"

MouseArea {
  property real flickX: 0

  enabled: instrItem.enabled && NOO.fingerPixels() * 4 > height && instrItem.scale === 1.0

  width: instrItem.width; height: instrItem.height
  z: 99

  onClicked: {
    var sc = (NOO.fingerPixels() * 4) / height
    instrItem.scale = sc
    flickX = (mouse.x / width) * (instrItem.width * sc - width)
  }

  onEnabledChanged: {
    if (!instrItem.enabled)
      instrItem.scale = 1
  }

  property var scoreWrap: score.parent
  Rectangle { // covers left side of zoomed-in score
    parent: nootkaWindow.contentItem
    width: (scoreWrap.width - scoreWrap.width * scoreWrap.scale) / 2
    height: scoreWrap.height * scoreWrap.scale
    y: scoreWrap.y
    color: NOO.alpha(activPal.base, 230)
    z: 5
  }
  Rectangle { // covers right side of zoomed-in score
    parent: nootkaWindow.contentItem
    width: (scoreWrap.width - scoreWrap.width * scoreWrap.scale) / 2
    height: scoreWrap.height * scoreWrap.scale
    x: parent.width - width
    y: scoreWrap.y
    color: NOO.alpha(activPal.base, 230)
    z: 5
  }

  Item { // overlay
    visible: instrItem.scale > 1
    enabled: instrItem.enabled
    parent: nootkaWindow.contentItem // NOTE valid only for Android
    width: parent.width; height: parent.height - instrItem.height * instrItem.scale
    z: 98
    MouseArea {
      anchors.fill: parent
      onClicked: instrItem.scale = 1
    }
  }
}
