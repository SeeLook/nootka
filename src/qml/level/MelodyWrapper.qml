/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0
import "../score"


Item {
  property int nr: 0
  property alias title: titleText.text
  property alias composer: composerText.text
  property alias score: score
  property alias scoreObj: score.scoreObj

  height: Noo.fontSize() * 10

  Score {
    id: score
    anchors.fill: parent
    readOnly: true
    bgColor: nr === melListView.currentMelody ? Qt.tint(activPal.base, Noo.alpha(activPal.highlight, 50)) : activPal.base
  }

  Rectangle { // this is part of covering rectangle
    height: parent.height; width: parent.height
    rotation: -90;
    x: parent.width - scoreCover.width - width
    gradient: Gradient {
      GradientStop { position: 0.0; color: "transparent" }
      GradientStop { position: 0.75; color: score.bgColor }
    }
  }
  Rectangle {
    id: scoreCover
    height: parent.height; width: parent.width - parent.height * 4
    anchors { right: parent.right }
    color: score.bgColor
  }

  Text {
    x: Noo.fontSize() / 2; y: Noo.fontSize() / 2
    font { family: "Sans"; bold: true }
    text: nr + 1; color: Noo.alpha(activPal.text, 150)
  }

  Text {
    id: titleText
    x: parent.width - scoreCover.width
    y: Noo.fontSize() / 4
    font { bold: true; pixelSize: Noo.fontSize() * 1.3 }
    color: wrapArea.containsMouse ? activPal.text : Noo.alpha(activPal.text, 150)
    width: Noo.fontSize() * 25; elide: Text.ElideRight
    Behavior on color { enabled: GLOB.useAnimations; ColorAnimation {} }
  }

  Text {
    id: composerText
    anchors { right: parent.right; rightMargin: Noo.fontSize() / 4 }
    y: Noo.fontSize() * 1.5
    color: wrapArea.containsMouse ? activPal.text : Noo.alpha(activPal.text, 150)
    maximumLineCount: 1
    Behavior on color { enabled: GLOB.useAnimations; ColorAnimation {} }
  }

  MouseArea {
    id: wrapArea
    anchors.fill: parent
    hoverEnabled: true
    onClicked: melListView.currentMelody = nr
  }
}
