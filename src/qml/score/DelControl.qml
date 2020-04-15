/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
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
    enabled: !scoreObj.singleNote && scoreObj.allowAdding && !scoreObj.readOnly
    onEditModeChanged: delControl.show = scoreObj.editMode && activeItem
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
    onScoreWasCleared: show = false
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
    x: activeItem ? -activeItem.width * score.scale - width * 1.3: 0
    width: controlButt.width; height: controlButt.height
    ControlButton {
      id: controlButt
      factor: delControl.factor
      font { family: "Nootka"; pixelSize: factor * 3 }
      text: "n"
      textColor: activPal.text
      onClicked: { scoreObj.insertNote(activeItem); delControl.show = false }
      Rectangle { y: factor / 10; x: factor / 2; height: factor; width: factor / 6; color: "green" }
      Rectangle { y: factor / 2; x: factor / 10; width: factor; height: factor / 6; color: "green" }
    }
  }
}
