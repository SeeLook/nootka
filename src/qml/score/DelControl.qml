/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0
import "../"


ControlBase {
  id: delControl

  visible: show

  property var activeItem: null

  Connections {
    target: scoreObj
    onActiveNoteChanged: {
      if (scoreObj.activeNote) {
          if (!scoreObj.readOnly && activeItem != scoreObj.activeNote) {
              activeItem = scoreObj.activeNote
              delControl.x = (activeItem.x + activeItem.width) * score.scale
              delControl.y = activeItem.staffItem.y + score.scale * 6
              delControl.active = true
          }
      } else if (activeItem) {
          delControl.active = false
      }
    }
  }

  hideTimer.onTriggered: activeItem = null

  component: Component {
    id: contentComp
    Column {
      ControlButton {
        factor: delControl.factor
        font { family: "Nootka"; pixelSize: factor * 3 }
        text: "o"
        textColor: "red"
        onClicked: { scoreObj.deleteNote(activeItem); show = false }
      }
    }
  }

  TipRect {
    y: score.scale * 24
    width: childrenRect.width; height: childrenRect.height
    ControlButton {
      factor: delControl.factor
      font { family: "Nootka"; pixelSize: factor * 3 }
      text: "n"
      textColor: "green"
      onClicked: { scoreObj.insertNote(activeItem); delControl.show = false }
    }
  }
}
