/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Score 1.0
import "../"
import "../score"


Tflickable {
  height: parent.height
  contentHeight: rangeGrid.height + NOO.factor() * 2
  contentWidth: Math.max(width, NOO.factor() * 35)

  Connections {
    target: creator
    onUpdateLevel: {
      rangeScore.setNote(0, creator.loNote)
      rangeScore.setNote(1, creator.hiNote)
    }
  }

  Grid {
    id: rangeGrid
    columns:  parent.width > NOO.factor() * 50 && GLOB.instrument.isGuitar? 2 : 1
    width: parent.width
    spacing: NOO.factor() / 4
    horizontalItemAlignment: Grid.AlignHCenter
    topPadding: NOO.factor()

    Tframe {
        width: rangeGrid.columns === 1 ? parent.width * 0.98 : parent.width * 0.49
        Column {
          width: parent.width
          spacing: NOO.factor() / 2
          Text { text: qsTr("note range:"); color: activPal.text; anchors.horizontalCenter: parent.horizontalCenter }
          Item {
            height: NOO.factor() * 20
            width: NOO.factor() * 12
            anchors.horizontalCenter: parent.horizontalCenter
            Score {
              id: rangeScore
              anchors.fill: parent
              scale: height / firstStaff.linesCount
              scoreObj.editMode: true
              meter: Tmeter.NoMeter
              clef: creator.clef
              Component.onCompleted: {
                rangeScore.addNote(creator.loNote)
                rangeScore.addNote(creator.hiNote)
              }
              Connections {
                target: rangeScore.firstStaff.clef
                onClicked: creator.clef = rangeScore.clef
              }
              Connections {
                target: rangeScore.scoreObj
                onClicked: {
                  creator.loNote = rangeScore.scoreObj.noteAt(0)
                  creator.hiNote = rangeScore.scoreObj.noteAt(1)
                }
              }
            }
          }
          Tile {
            visible: GLOB.instrument.isGuitar
            TcuteButton {
              text: qsTr("adjust fret range")
              anchors.horizontalCenter: parent.horizontalCenter
              onClicked: creator.adjustFretsToScale()
            }
            description: qsTr("Adjust fret range in a level to currently selected note range")
          }
        }
    }

    Column {
      visible: GLOB.instrument.isGuitar
      width: rangeGrid.columns === 1 ? parent.width * 0.98 : parent.width * 0.49
      spacing: NOO.factor() / 2
      Tframe {
          width: parent.width
          Column {
            width: parent.width
            spacing: NOO.factor() / 2
            Text { text: qsTr("fret range:"); color: activPal.text; anchors.horizontalCenter: parent.horizontalCenter }
            Row {
              anchors.horizontalCenter: parent.horizontalCenter
              spacing: NOO.factor() / 2
              Text { text: qsTr("from"); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
              TspinBox {
                id: loFretSpin
                from: 0; to: GLOB.fretNumber
                value: creator.loFret
                onValueModified: {
                  creator.loFret = loFretSpin.value
                  if (loFretSpin.value > hiFretSpin.value)
                    creator.hiFret = loFretSpin.value
                }
              }
              Text { text: qsTr("to"); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
              TspinBox {
                id: hiFretSpin
                from: 0; to: GLOB.fretNumber
                value: creator.hiFret
                onValueModified: {
                  creator.hiFret = hiFretSpin.value
                  if (hiFretSpin.value < loFretSpin.value)
                    creator.loFret = hiFretSpin.value
                }
              }
            }
            Tile {
              TcuteButton {
                text: qsTr("adjust note range")
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: creator.adjustNotesToFretRange()
              }
              description: qsTr("Adjust note range in a level to currently selected fret range")
            }
          }
      }
      Item { width: parent.width; height: rangeGrid.columns === 1 ? 0 : NOO.factor() * 3 } // spacer only for 2 columns
      Tile {
        description: qsTr("Uncheck strings if you want to skip them in an exam.")
        Column {
          width: parent.width
          spacing: NOO.factor() / 4
          Text { text: qsTr("available strings:"); color: activPal.text; anchors.horizontalCenter: parent.horizontalCenter }
          Grid {
            spacing: NOO.factor()
            anchors.horizontalCenter: parent.horizontalCenter
            columns: Math.ceil(GLOB.tuning.stringNumber / 2.0)
            horizontalItemAlignment: Grid.AlignHCenter
            Repeater {
              model: GLOB.tuning.stringNumber
              TcheckBox {
                checked: creator.usedStrings & Math.pow(2, index)
                text: index + 1
                textItem.topPadding: NOO.factor() * 0.5
                font { family: "nootka"; pixelSize: NOO.factor() * 3 }
                onClicked: creator.usedStrings = checked ? creator.usedStrings | Math.pow(2, index) : creator.usedStrings & ~Math.pow(2, index)
              }
            }
          }
        }
      }
    }
  }
}
