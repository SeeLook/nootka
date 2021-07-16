/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import "../"


Column {
  id: saxCol

  property alias transposition: transpose.outShift

  spacing: NOO.factor()
  anchors.horizontalCenter: parent.horizontalCenter
  width: parent.width - NOO.factor() * 2

  Text {
    width: parent.width
    font { pixelSize: NOO.factor() * 1.2; bold: true }
    wrapMode: Text.WordWrap
    horizontalAlignment: Text.AlignHCenter
    color: activPal.text
    text: qsTr("Saxophones are transposing instruments.<br>It means that note pitch in the score doesn't correspond directly to note which is played - it transposes by interval (number of semitones) according to kind of saxophone.")
  }

  Tile {
    width: transCol.width + NOO.factor() * 2
    bgColor: nootkaWindow.bgColor
    bgBorder { width: 2; color: nootkaWindow.labelColor }
    Column {
      id: transCol
      width: Math.min(NOO.factor() * 50, saxCol.width * 0.8)
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: NOO.factor()
      Transposition {
        id: transpose
        anchors.horizontalCenter: parent.horizontalCenter
        shift: NOO.instr(nootkaWindow.instrument).transposition
      }
      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        font { pixelSize: NOO.factor() * 2; bold: true }
        text: NOO.noteName(NOO.note(13 + transpose.outShift, false), 0, false)
        color: activPal.text
      }
    }
  }

  Text {
    text: "Saxophone support is new and not well tested,\nso please give some feedback or call a bug."
    color: "red"
    width: parent.width
    font { pixelSize: NOO.factor() * 1.2; bold: true }
    horizontalAlignment: Text.AlignHCenter
  }

  Connections {
    target: nootkaWindow
    onInstrumentChanged: transpose.shift = NOO.instr(nootkaWindow.instrument).transposition
  }
}
