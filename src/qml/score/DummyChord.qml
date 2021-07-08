/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0


TdummyChord {
  id: chordIt

  anchors.fill: parent

  Rectangle {
    anchors.fill: parent
    z: -1; radius: width / 4
    color: NOO.alpha(activPal.dimText, 30)
    Repeater {
      model: chordModel
      Text {
        scale: 1.2
        font { family: "Scorek"; pixelSize: 7 }
        text: noteHead
        y: headPos(index) - 15
        color: index === selected ? activPal.text : activPal.dimText
        Text {
          font { family: "Scorek"; pixelSize: 7 }
          color: activPal.dimText
          x: -width - 0.1
  //         text: score.alterText
        }
      }
    }
  }

  MouseArea {
    id: ma
    anchors.fill: parent
    onClicked: {
      importWindow.showChord(chordIt)
    }
  }
}
