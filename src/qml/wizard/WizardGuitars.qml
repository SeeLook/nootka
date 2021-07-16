/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import Score 1.0
import "../"
import "../score"


Column {
  spacing: NOO.factor()
  width: parent.width - NOO.factor() * 2

  Text {
    width: parent.width
    wrapMode: Text.WordWrap
    font { pixelSize: NOO.factor() * 1.2; bold: true }
    horizontalAlignment: Text.AlignHCenter
    color: activPal.text
    text: qsTr("Guitar notation uses the treble clef with the digit \"eight\" written below <br>(even if some editors are forgetting about this digit).<br>Try to understand this.")
  }
  Tile {
    width: scoreRow.width + NOO.factor() * 2
    bgColor: nootkaWindow.bgColor
    bgBorder { width: 2; color: nootkaWindow.labelColor }
    Row {
      id: scoreRow
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: NOO.factor(); leftPadding: NOO.factor()
      Repeater {
        model: 2
        Item {
          width: sc.width; height: sc.height * 0.5
          Score {
            id: sc
            y: (upperLine - 29) * scale + height * 0.05
            scale: height / firstStaff.linesCount
            height: Math.max(NOO.factor() * 18, nootkaWindow.height * 0.4); width: Math.max(NOO.factor() * 9, nootkaWindow.height * 0.2)
            bgColor: "transparent"
            meter: Tmeter.NoMeter
            readOnly: true
            clef: index === 1 ? Tclef.Treble_G_8down : Tclef.Treble_G
            scoreObj.showNoteNames: true
            scoreObj.nameColor: GLOB.nameColor
            scoreObj.nameStyle: GLOB.noteNameStyle
            Component.onCompleted: addNote(NOO.note(1, 1, 0, Trhythm.NoRhythm))
          }
        }
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
