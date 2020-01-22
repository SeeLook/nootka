/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka 1.0
import "../"


TipRect {
  id: fView
  clip: true
  visible: false
  width: instrItem.fretWidth * 1.5; height: instrItem.height * 1.3
  y: parent.height - height

  TguitarBg {
    id: gBg
    height: parent.height; width: instrItem.width * 1.3
    x: fView.x * -1.3
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
    anchors.fill: parent
    drag.target: fView
    drag.axis: Drag.XAxis
    cursorShape: drag.active ? Qt.DragMoveCursor : Qt.ArrowCursor
    onClicked: {
      gBg.pressedAt(gBg.x * -1 + mouse.x, mouse.y)
      instrItem.pressedAt(gBg.x / -1.3 + mouse.x, mouse.y / 1.3)
    }
  }
}

