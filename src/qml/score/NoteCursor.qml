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
  property alias alterText: alter.text

  visible: yPos > 0

  Rectangle { // highlight
    width: parent.width + 1
    height: parent.height
    x: -1
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
      model: (score.upperLine - 2) / 2
      AddLine { y: 2 * (index + 1) - 0.1; visible: yPos > 0 && index >= Math.floor((yPos - 1) / 2) }
  }
  Repeater { // middle lines
    model: score.clef === Tclef.PianoStaffClefs ? 2 : 0
    AddLine { y: score.upperLine + 10 + 2 * index - 0.1; visible: yPos == score.upperLine + 10 + 2 * index
    }
  }
  Repeater { // lower lines
      model: (score.firstStaff.height - score.upperLine - 12) / 2
      AddLine { 
        y: score.upperLine + (score.clef === Tclef.PianoStaffClefs ? 22 : 10) + 2 * index - 0.1
        visible: score.upperLine + (score.clef === Tclef.PianoStaffClefs ? 22 : 10) + index * 2 <= yPos
      }
  }

}
