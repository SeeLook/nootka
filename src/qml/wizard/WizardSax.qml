/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


Column {
  property alias transposition: transpose.outShift

  spacing: Noo.fontSize()
  anchors.horizontalCenter: parent.horizontalCenter
  width: parent.width * 0.9

  Text {
    width: parent.width
    font { pixelSize: Noo.fontSize() * 1.5; bold: true }
    wrapMode: Text.WordWrap
    horizontalAlignment: Text.AlignHCenter
    color: activPal.text
    text: qsTr("Saxophones are transposing instruments.<br>It means that note pitch in the score doesn't correspond directly to note which is played - it transposes by interval (number of semitones) according to kind of saxophone.")
  }
  Transposition {
    id: transpose
    anchors.horizontalCenter: parent.horizontalCenter
    shift: Noo.instr(nootkaWindow.instrument).transposition
  }
  Text {
    anchors.horizontalCenter: parent.horizontalCenter
    font { pixelSize: Noo.fontSize() * 2; bold: true }
    text: Noo.noteName(Noo.note(13 + transpose.outShift, false), 0, false)
    color: activPal.text
  }

  Text {
    text: "Saxophone support is new and not well tested,\nso please give some feedback or call a bug."
    color: "red"
    width: parent.width
    font { pixelSize: Noo.fontSize() * 1.5; bold: true }
    horizontalAlignment: Text.AlignHCenter
  }

  Connections {
    target: nootkaWindow
    onInstrumentChanged: transpose.shift = Noo.instr(nootkaWindow.instrument).transposition
  }
}
