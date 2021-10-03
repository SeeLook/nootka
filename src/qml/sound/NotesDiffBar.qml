/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Nootka 1.0
import "../"


TnotesBarItem {
  id: nBar

  expectedNote: score.note

  z: score.z
  y: score.y + score.height
  width: score.width; height: nootkaWindow.height / 20

  Rectangle { // background
    z: -1
    anchors.fill: parent
    color: NOO.alpha(activPal.window, 250)
  }

  GlowRect {
    color: Qt.tint(activPal.base, NOO.alpha((notesDiff === 0 ? GLOB.correctColor: GLOB.wrongColor), 50))
    x: expectedX; y: (parent.height - height) / 2
    width: nBar.height - NOO.factor() / 2; height: width
    Text {
      y: -height * 0.2; anchors.horizontalCenter: parent.horizontalCenter
      font { family: "Scorek"; pixelSize: nBar.height / 2 }
      color: activPal.text
      text: expectedName
    }
  }

  GlowRect {
    visible: detectedX > 0
    color: activPal.base
    x: detectedX; y: (parent.height - height) / 2
    width: nBar.height - NOO.factor() / 2; height: width
    Text {
      y: -height * 0.2; anchors.horizontalCenter: parent.horizontalCenter
      font { family: "Scorek"; pixelSize: nBar.height / 2 }
      color: activPal.text
      text: detectedName
    }
    scale: isPlaying ? 1.5 : 1
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    Behavior on x { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  }

  Component.onCompleted: setAmbitus(GLOB.loNote(), GLOB.hiNote())

  Connections {
    target: GLOB
    onTuningChanged: setAmbitus(GLOB.loNote(), GLOB.hiNote())
  }

}
