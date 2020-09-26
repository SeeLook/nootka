/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Window 2.2

import Nootka 1.0
import "../"


Item {
  id: intoBar

  property real deviation: 0.0
  property bool active: true
  property alias pitchText: noteText.text

  property real tickWidth: Screen.pixelDensity * 0.5
  property real tickGap: tickWidth * 1.4

  TtickColors { id: tc; width: (intoBar.width - textWrap.width * 2) / 2; divisor: tickGap + tickWidth }

  Repeater {
    id: iRepLeft
    model: tc.width / tc.divisor
    Rectangle {
      color: active ? (deviation < 0 && iRepLeft.model - index <= (deviation * -2 * iRepLeft.model) ? tc.colorAt(iRepLeft.model - index) : activPal.text) : disdPal.text
      width: tickWidth
      radius: tickWidth / 2
      height: tickWidth * 1.5 + ((intoBar.height - tickWidth * 4) / iRepLeft.model) * (iRepLeft.model - index)
      y: (parent.height - height) / 2
      x: (index * tickGap) + (index + 2) * tickWidth
    }
  }

  TipRect {
    id: textWrap
    width: height * 1.7; height: parent.height * 1; radius: height / 8
    color: Qt.tint(activPal.base, Noo.alpha(active ? (dev > 0 ? (dev > 0.3 ? "red" : (dev > 0.1 ? "yellow" : "lime")) : activPal.base) : disdPal.base, 50))
    Behavior on color { ColorAnimation { duration: 150 }}
    anchors.top: parent.Top
    x: (intoBar.width - width) / 2
    property real dev: Math.abs(deviation)
    Text {
      id: noteText
      y: height * -0.26; x: (parent.width - width) / 2
      font { family: "Scorek"; pixelSize: parent.height * 0.8 }
      color: activPal.text; textFormat: Text.StyledText
    }
  }

  Repeater {
    id: iRepRight
    model: tc.width / tc.divisor
    Rectangle {
      color: active ? (deviation > 0 && index <= (deviation * 2 * iRepRight.model) ? tc.colorAt(index) : activPal.text) : disdPal.text
      width: tickWidth
      radius: tickWidth / 2
      height: tickWidth * 1.5 + ((intoBar.height - tickWidth * 4) / iRepRight.model) * index
      y: (parent.height - height) / 2
      x: textWrap.x + textWrap.width * 1.5 + (index * tickGap) + (index + 2) * tickWidth
    }
  }

}
