/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0
import "../"


Flickable {
  width: parent.width; height: parent.height
  clip: true
  contentHeight: rangeGrid.height + nootkaWindow.fontSize * 2
  contentWidth: Math.max(width, nootkaWindow.fontSize * 35)

  ScrollBar.vertical: ScrollBar { active: !Noo.isAndroid() }

  Grid {
    id: rangeGrid
    columns:  parent.width > nootkaWindow.fontSize * 50 ? 2 : 1
    width: parent.width
    spacing: nootkaWindow.fontSize / 4
    horizontalItemAlignment: Grid.AlignHCenter
    topPadding: nootkaWindow.fontSize

    Frame {
        width: rangeGrid.columns === 1 ? parent.width * 0.98 : parent.width * 0.49
        Column {
          width: parent.width
          spacing: nootkaWindow.fontSize / 2
          Text { text: qsTr("note range:"); color: activPal.text; anchors.horizontalCenter: parent.horizontalCenter }
          Score {
            id: rangeScore
            height: nootkaWindow.fontSize * 20
            width: nootkaWindow.fontSize * 12
            anchors.horizontalCenter: parent.horizontalCenter
            meter: Tmeter.NoMeter
            Component.onCompleted: {
              rangeScore.addNote(Noo.note(1, 1, 0, Trhythm.NoRhythm))
              rangeScore.addNote(Noo.note(1, 2, 0, Trhythm.NoRhythm))
            }
          }
          Tile {
            Button {
              text: qsTr("adjust fret range")
              anchors.horizontalCenter: parent.horizontalCenter
            }
            description: qsTr("Adjust fret range in a level to currently selected note range")
          }
        }
    }

    Column {
      width: rangeGrid.columns === 1 ? parent.width * 0.98 : parent.width * 0.49
      spacing: nootkaWindow.fontSize / 2
      Frame {
          width: parent.width
          Column {
            width: parent.width
            spacing: nootkaWindow.fontSize / 2
            Text { text: qsTr("fret range:"); color: activPal.text; anchors.horizontalCenter: parent.horizontalCenter }
            Row {
              anchors.horizontalCenter: parent.horizontalCenter
              spacing: nootkaWindow.fontSize / 2
              Text { text: qsTr("from"); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
              SpinBox { id: fromFretSpin; from: 0; to: GLOB.fretNumber; value: 0 }
              Text { text: qsTr("to"); color: activPal.text; anchors.verticalCenter: parent.verticalCenter }
              SpinBox { id: toFretSpin; from: 0; to: GLOB.fretNumber; value: 3 }
            }
            Tile {
              Button {
                text: qsTr("adjust note range")
                anchors.horizontalCenter: parent.horizontalCenter
              }
              description: qsTr("Adjust note range in a level to currently selected fret range")
            }
          }
      }
      Item { width: parent.width; height: rangeGrid.columns === 1 ? 0 : nootkaWindow.fontSize * 3 } // spacer only for 2 columns
      Tile {
        description: qsTr("Uncheck strings if you want to skip them in an exam.")
        Column {
          width: parent.width
          spacing: nootkaWindow.fontSize / 4
          Text { text: qsTr("available strings:"); color: activPal.text; anchors.horizontalCenter: parent.horizontalCenter }
          Grid {
            spacing: nootkaWindow.fontSize / 4
            anchors.horizontalCenter: parent.horizontalCenter
            columns: 3
            horizontalItemAlignment: Grid.AlignHCenter
            Repeater {
              model: 6
              TcheckBox {
                checked: true
                text: index + 1
                font { family: "nootka"; pixelSize: nootkaWindow.fontSize * 2 }
              }
            }
          }
        }
      }
    }
  }
}
