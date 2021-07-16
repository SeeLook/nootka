/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import "../"


Column {
  spacing: NOO.factor()
  anchors.horizontalCenter: parent.horizontalCenter
  width: parent.width * 0.9

  Text {
    text: qsTr("Notes for bandoneon are written on the grand staff - two staves joined with brace.") + "<br>"
    + qsTr("All notes on the upper staff with treble clef are played with right hand and the notes on the lower one with bass clef are played with the left hand.")
    + "<br><br>"
    + qsTr("Direction of bellows movement is marked with bowing symbols:")
    width: parent.width; wrapMode: Text.WordWrap
    color: activPal.text
    horizontalAlignment: Text.AlignHCenter
    font { pixelSize: NOO.factor() * 1.2; bold: true }
  }

  property real maxBowW: 0
  Column {
    anchors.horizontalCenter: parent.horizontalCenter
    Repeater {
      model: [ "\uE610", "\uE612" ]
      Tile {
        property color accent: index === 0 ? "blue" : "#FF00FF"
        width: maxBowW + NOO.factor() * 3
        bgColor: Qt.tint(accent, NOO.alpha(activPal.base, 230))
        bgBorder { width: 2; color: accent }
        Row {
          anchors.horizontalCenter: parent.horizontalCenter
          spacing: NOO.factor() * 2
          Text {
            anchors.verticalCenter: parent.verticalCenter
            text: modelData
            font { pixelSize: NOO.factor() * 2; family: "Nootka" }
            color: accent
            leftPadding: index === 0 ? NOO.factor() / 2 : 0
          }
          Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "- " + NOO.TR("Bandoneon", index === 0 ? "Bellows is opening." : "Bellows is closing.").toLowerCase().replace(".", "")
            font { pixelSize: NOO.factor() * 1.2; bold: true }
            color: activPal.text
          }
          Component.onCompleted: maxBowW = Math.max(maxBowW, width)
        }
      }
    }
  }
}
