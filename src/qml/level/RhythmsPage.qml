/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Dialogs 1.0
import "../"


Tflickable {
  id: rtmPage

  contentHeight: rtmCol.height

  Column {
    id: rtmCol
    width: parent.width; topPadding: Noo.fontSize() / 2; spacing: Noo.fontSize() / 2
    enabled: creator.randMelody !== 2 && creator.melodyLen > 1

    Text {
      text: qsTr("Select any rhythm and time signature to be used in this level, otherwise melodies will not have rhythmic values, just bare note-heads.")
      anchors.horizontalCenter: parent.horizontalCenter
      width: parent.width * 0.96
      font { pixelSize: Noo.fontSize() * 0.8; bold: true }
      textFormat: Text.RichText; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap
      color: enabled ? ((rtmSel.basicMask || rtmSel.dotsMask) && metersSel.meters ? activPal.text : "red") : disdPal.text
    }

    Row {
      width: parent.width; spacing: Noo.fontSize() / 4
      Tile {
        id: selTile
        anchors.horizontalCenter: undefined
        width: rtmSel.width * 1.2; height: Math.max(rtmPage.height - Noo.fontSize() * 2, rCol.height)
        RhythmSelector {
          x: width / 10
          id: rtmSel
          height: selTile.height - Noo.fontSize() * 2
          basicMask: creator.basicRhythms
          dotsMask: creator.dotsRhythms
          onBasicGroupChanged: creator.basicRhythms = basicMask
          onDotsGroupChanged: creator.dotsRhythms = dotsMask
//           useRests: creator.useRests
//           onRestsClicked: creator.useRests = useRests
//           useTies: creator.useTies
//           onTiesClicked: creator.useTies = useTies
        }
      }
      Column {
        id: rCol
        width: parent.width - selTile.width - Noo.fontSize() / 4; spacing: Noo.fontSize() / 4
        enabled: rtmSel.basicMask || rtmSel.dotsMask
        Tile {
          MeterSelector {
            id: metersSel
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - Noo.fontSize() * 2
            meters: creator.meters
            onMetersModified: creator.meters = meters
          }
          description: qsTr("Time signatures to use in this level.")
        }
        Tile {
          description: qsTr("Number of measures in a melody. If 'variable' is set, it will grow up to the given value as an exercise or an exam is progressing.")
          Row {
            id: measRow
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: Noo.fontSize()
            Text {
              anchors.verticalCenter: parent.verticalCenter
              text: Noo.TR("MelGenDialog", "Measures number")
              color: enabled ? activPal.text : disdPal.text
            }
            TspinBox {
              id: barNrSpin
              from: 2; to: 32
              value: creator.barNumber
              onValueModified: creator.barNumber = value
            }
            TcheckBox {
              text: qsTr("variable")
              checked: creator.variableBarNr
              onClicked: creator.variableBarNr = checked
            }
          }
        }
        RhythmDiversityTile {
          diversity: creator.rhythmDiversity
          onDiversityModified: creator.rhythmDiversity = diversity
        }
      }
    }
  }

  //     Column {
//       id: settCol
//       anchors.verticalCenter: parent.verticalCenter
//       TcheckBox {
//         id: restsChB
//         text: qsTr("rests")
//         onClicked: restsClicked()
//       }
//       TcheckBox {
//         id: tiesChB
//         text: qsTr("ties")
//         onClicked: tiesClicked()
//       }
//     }

}

