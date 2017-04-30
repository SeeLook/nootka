/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7

import Score 1.0


Item {
  id: noteCursor

  height: parent ? parent.height : 0
  width: parent ? parent.width - parent.alterWidth : 0

  property color color: GLOB.noteCursorColor
  property real yPos: 0
  property real upperLine: 16
  property alias alterText: alter.text

  visible: yPos > 0

  Rectangle { // highlight
    anchors.fill: parent
    color: Qt.rgba(noteCursor.color.r, noteCursor.color.g, noteCursor.color.b, 0.1)
    z: -10
  }

  Text {
      id: head
      scale: 1.2
      font { family: "Scorek"; pixelSize: 7 }
      text: "\uf4be"
      y: yPos - 15
      color: noteCursor.color

      Text {
        id: alter
        font { family: "Scorek"; pixelSize: 7 }
        color: noteCursor.color
        x: head.x - width - 0.1
      }
  }

  Repeater { // upper lines
      model: (upperLine - 2) / 2
      Rectangle {
        x: - 1.0
        y: 2 * (index + 1) - 0.1
        height: 0.2
        width: 4.0
        color: noteCursor.color
        visible: yPos > 0 && index >= Math.floor((yPos - 1) / 2)
      }
  }
  Repeater { // lower lines
      model: (parent.height - upperLine - 12) / 2
      Rectangle {
        x: - 1.0
        y: upperLine + 10.0 + 2 * index - 0.1
        height: 0.2
        width: 4.0
        color: noteCursor.color
        visible: upperLine + 10.0 + index * 2 <= yPos
      }
  }

}
