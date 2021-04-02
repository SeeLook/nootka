/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0
import "../"
import "../score"


Column {
  property alias transposition: transpose.outShift
  property alias clef: score.clef

  width: parent ? parent.width : 0
  spacing: NOO.factor()

  function getNote(noteNr) {
    return score.scoreObj.noteAt(noteNr)
  }

  Text {
    width: parent.width
    font { pixelSize: NOO.factor() * (NOO.isAndroid() ? 1.2 : 1.5); bold: true }
    wrapMode: Text.WordWrap
    horizontalAlignment: Text.AlignHCenter
    color: activPal.text
    text: qsTr("Select a clef and scale of notes appropriate for your instrument.")
  }
  Tile {
    width: score.width + NOO.factor() * 3
    bgColor:  nootkaWindow.bgColor
    bgBorder { width: 2; color: nootkaWindow.labelColor }
    Item {
      width: score.width; height: score.height
      anchors.horizontalCenter: parent.horizontalCenter
      Score {
        id: score
        height: Math.max(NOO.factor() * 18, nootkaWindow.height * 0.4); width: Math.max(NOO.factor() * 12, nootkaWindow.height / 4)
        bgColor: "transparent"
        meter: Tmeter.NoMeter
        scoreObj.editMode: true
        scoreObj.showNoteNames: true
        scoreObj.nameColor: GLOB.nameColor
        scoreObj.nameStyle: GLOB.noteNameStyle
        Component.onCompleted: {
          addNote(scoreObj.lowestNote())
          addNote(scoreObj.highestNote())
        }
        onClefChanged: {
          score.scoreObj.setNote(0, score.scoreObj.lowestNote())
          score.scoreObj.setNote(1, score.scoreObj.highestNote())
        }
      }
    }
  }
  Transposition {
    id: transpose
    shift: 0
  }
}
