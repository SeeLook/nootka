/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"

TipRect {
  id: cntDwnItem

  property int tickCount: 0

  width: row.width + Noo.fontSize() * 4; height: nootkaWindow.height / 10
  x: height * 2; y: Noo.fontSize() / 2
  z: 100
  color: Qt.tint(activPal.text, Noo.alpha("#00a0a0", 100))
  visible: counter <= tickCount && (SOUND.listening || SOUND.playing) && SOUND.metroRunning

  // private
  property int counter: 1

  Connections {
    target: tempoBar
    onCntChanged: {
      if (SOUND.metroRunning)
        counter++
    }
  }

  Row {
    id: row
    anchors.centerIn: parent
    spacing: Noo.fontSize() * 2
    Repeater {
      id: cntRep
      model: tickCount
      Text {
        y: height * 0.07
        color: index + 1 === counter ? activPal.highlight : activPal.base
        text: index % tempoBar.countTo + 1
        font { pixelSize: cntDwnItem.height * 0.8; family: "Scorek" }
      }
    }
  }

}


