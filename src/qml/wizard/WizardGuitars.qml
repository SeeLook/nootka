/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0
import "../"
import "../score"


Column {
  spacing: Noo.fontSize()
  width: parent.width

  Text {
    width: parent.width
    wrapMode: Text.WordWrap
    font { pixelSize: Noo.fontSize() * 1.2; bold: true }
    horizontalAlignment: Text.AlignHCenter
    color: activPal.text
    text: qsTr("Guitar notation uses the treble clef with the digit \"eight\" written below <br>(even if some editors are forgetting about this digit).<br>Try to understand this.")
  }
  Row {
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: Noo.fontSize()
    Repeater {
      model: 2
      Score {
        id: sc
        height: Math.max(Noo.fontSize() * 18, nootkaWindow.height * 0.4); width: Math.max(Noo.fontSize() * 9, nootkaWindow.height * 0.2)
        meter: Tmeter.NoMeter
        readOnly: true
        clef: index === 1 ? Tclef.Treble_G_8down : Tclef.Treble_G
        scoreObj.showNoteNames: true
        scoreObj.nameColor: GLOB.nameColor
        scoreObj.nameStyle: GLOB.noteNameStyle
        Component.onCompleted: addNote(Noo.note(1, 1, 0, Trhythm.NoRhythm))
      }
    }
  }
  Text {
    width: parent.width
    wrapMode: Text.WordWrap; textFormat: Text.StyledText
    horizontalAlignment: Text.AlignHCenter
    color: activPal.text
    text: "<b><font size=\"5\">" + qsTr("Both pictures above show the same note!%1(note c in one-line octave)").arg("</font></b><br>")
  }
}
