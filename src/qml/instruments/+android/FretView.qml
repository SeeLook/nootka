/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka 1.0
import "../"


TipRect {
  id: fView
  z: 99
  height: Math.max(Noo.fingerPixels() * 4, instrItem.height * 1.1)
  width: instrItem.fretWidth * 1.5 * vScale
  y: parent.height - height
  scale: 0
  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation {} }

  property real vScale: height / instrItem.height

  Item { // overlay
    parent: fView.parent
    anchors.fill: parent
    z: 98
    visible: fView.scale === 1
    MouseArea {
      anchors.fill: parent
      onClicked: fView.scale = 0
    }
  }

  Connections {
    target: instrItem
    onNoteWasSet: gBg.note = instrItem.note
  }

  Item {
    anchors.fill: parent
    clip: true
    TguitarBg {
      id: gBg
      height: parent.height; width: instrItem.width * vScale
      x: fView.x * -vScale
      note: instrItem.note
      Image { // rosette/pickup
        cache: false
        source: GLOB.instrument.isGuitar ? Noo.pix(GLOB.instrument.type === Tinstrument.ClassicalGuitar ? "rosette" : "pickup") : ""
        height: parent.height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 1.55 : 1.3)
        width: height * (sourceSize.width / sourceSize.height)
        x: GLOB.instrument.type === Tinstrument.ClassicalGuitar ? gBg.fbRect.width - height * 0.25 : parent.width * 0.87
        y: parent.height - height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ?  0.95 : 0.88)
        z: -1
      }
    }
    MouseArea {
      enabled: fView.scale === 1
      anchors.fill: parent
      drag.target: fView
      drag.minimumX: 0; drag.maximumX: instrItem.width - width
      drag.axis: Drag.XAxis
      cursorShape: drag.active ? Qt.DragMoveCursor : Qt.ArrowCursor
      onClicked: {
        gBg.pressedAt(gBg.x * -1 + mouse.x, mouse.y)
        instrItem.pressedAt(gBg.x / -vScale + mouse.x, mouse.y / vScale)
      }
    }
  }
}

