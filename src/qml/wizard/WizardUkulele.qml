/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import Score 1.0
import Nootka 1.0
import "../"
import "../score"


Column {
  id: saxCol

  spacing: NOO.factor()
  anchors.horizontalCenter: parent.horizontalCenter
  width: parent.width - NOO.factor() * 2

  property var tuning: NOO.tuning(Ttune.Ukulele_GCEA)

  Tile {
    width: score.width + NOO.factor() * 2
    bgColor: nootkaWindow.bgColor
    bgBorder { width: 2; color: nootkaWindow.labelColor }
    Item {
      width: score.width; height: score.height * 0.8
      anchors.horizontalCenter: parent.horizontalCenter
      Score {
        id: score
        y: (upperLine - 22) * scale + height * 0.05
        height: Math.max(NOO.factor() * 15, nootkaWindow.height * 0.35); width: height
        scale: height / firstStaff.linesCount
        bgColor: "transparent"
        scoreObj.editMode: false
        scoreObj.showNoteNames: true
        scoreObj.nameColor: GLOB.nameColor
        scoreObj.nameStyle: GLOB.noteNameStyle
        clef: Tclef.Treble_G
        meter: Tmeter.NoMeter
        Component.onCompleted: {
          for (var i = 1; i < 5; ++i) {
            addNote(tuning.str(i))
            strComp.createObject(score.scoreObj.lastNote, { "text": i })
          }
        }
      }
      Text {
        anchors { top: parent.top; horizontalCenter: parent.horizontalCenter }
        font.bold: true; text: NOO.TR("InstrumentPage", "tuning") + ": " + tuning.name
      }
    }
  }

  Component {
    id: strComp
    Text {
      y: 11; x: (parent.width - width) / 2; color: activPal.text; font { family: "Nootka"; pixelSize: 5 }
    }
  }

  Text {
    width: parent.width - NOO.factor() * 2
    font { pixelSize: NOO.factor() * 1.2; bold: true }
    wrapMode: Text.WordWrap; horizontalAlignment: Text.AlignHCenter
    color: activPal.text
    text: qsTr("Ukulele tuning is known as 'reentrant' (non linear) - which means that descending order of string pitches is not kept. It may be confusing at very beginning during playing from musical scores.",
               "In this context 'reentant' and 'non linear' can be used interchangeably so you may translate just one of them.")
  }
}
