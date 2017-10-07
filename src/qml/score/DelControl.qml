/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0


ControlBase {
  id: delControl

  x: show && scoreObj.lastNote ? (scoreObj.lastNote.x + scoreObj.lastNote.width) * score.scale : score.width + Noo.fontSize() + width
  y: score.lastStaff.y + score.scale * 6

  component: Component {
    id: contentComp
    Column {
      ControlButton {
        factor: delControl.factor
        font { family: "Nootka"; pixelSize: factor * 3 }
        text: "o"
        textColor: "red"
        onClicked: { scoreObj.deleteLastNote(); show = false }
      }
    }
  }
}
