/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0
import "../score"


Item {
  property int nr: 0
  property alias score: score
  property alias scoreObj: score.scoreObj

  height: Noo.fontSize() * 10

  Score {
    id: score
    anchors.fill: parent
    readOnly: true
    bgColor: nr === melListView.currentMelody ? Qt.tint(activPal.base, Noo.alpha(activPal.highlight, 50)) : activPal.base
  }

  Text {
    x: Noo.fontSize() / 2; y: Noo.fontSize() / 2
    font { family: "Sans"; bold: true }
    text: nr + 1; color: Noo.alpha(activPal.text, 150)
  }

  MouseArea {
    anchors.fill: parent
    onClicked: {
      melListView.currentMelody = nr
    }
  }
}
