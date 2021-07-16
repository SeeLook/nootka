/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import Score 1.0
import "../"
import "../score"


Column {
  spacing: NOO.factor() * (NOO.isAndroid() ? 1 : 3)
  anchors.horizontalCenter: parent.horizontalCenter
  width: parent.width

  Text {
    anchors.horizontalCenter: parent.horizontalCenter
    width: parent.width - NOO.factor() * 2
    text: qsTr("Notes for piano are written on the grand staff - two staves joined with brace. Usually the upper one, with treble clef is for right hand and the lower one with bass clef is for left hand.")
    + "<br>" + qsTr("Although for exercise purposes and also due to Nootka is supporting only single voice, sometimes just one of the staves can be used as well.")
    color: activPal.text; wrapMode: Text.WordWrap
    font { pixelSize: NOO.factor() * 1.2; bold: true }
  }

  Tile {
    width: score.width + NOO.factor() * 2
    bgColor: nootkaWindow.bgColor
    bgBorder { width: 2; color: nootkaWindow.labelColor }
    Item {
      width: score.width; height: score.height * 0.8
      anchors.horizontalCenter: parent.horizontalCenter
      Score {
        id: score
        y: (upperLine - 25) * scale + height * 0.05
        height: Math.max(NOO.factor() * 15, nootkaWindow.height * 0.35); width: height * 1.6
        scale: height / firstStaff.linesCount
        bgColor: "transparent"
        scoreObj.editMode: false
        scoreObj.showNoteNames: true
        scoreObj.nameColor: GLOB.nameColor
        scoreObj.nameStyle: GLOB.noteNameStyle
        clef: Tclef.PianoStaffClefs
        Component.onCompleted: {
          for (var n = 1; n < 8; ++n)
            addNote(NOO.setUpperStaff(NOO.note(n, 0, 0, 5), false))
          addNote(NOO.setUpperStaff(NOO.note(1, 1, 0, 5), false))
          for (var n = 1; n < 8; ++n)
            addNote(NOO.note(n, 1, 0, 5))
          addNote(NOO.note(1, 2, 0, 5))

        }
      }
    }
  }
}
