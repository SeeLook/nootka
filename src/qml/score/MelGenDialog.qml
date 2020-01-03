/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Dialogs 1.0
import "../"
import "../level"


TmelGenItem {
  id: melGenItem

  width: parent.width; height: parent.height

  rhythmSelector: rtmSel

  Tflickable {
    height: parent.height
    contentHeight: mainCol.height

    Column {
      id: mainCol
      leftPadding: Noo.fontSize() / 2
      Row {
        Tile {
          id: selTile
          anchors.horizontalCenter: undefined
          width: rtmSel.width * 1.2; height: Math.max(melGenItem.height, rCol.height)
          RhythmSelector {
            id: rtmSel
            x: width / 10
            height: selTile.height * 0.95
          }
        }
        Column {
          id: rCol
          width: melGenItem.width - rtmSel.width * 1.2 - Noo.fontSize()
          Tile {
            Row {
              id: measRow
              anchors.horizontalCenter: parent.horizontalCenter
              spacing: Noo.fontSize()
              Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Measures number")
                color: enabled ? activPal.text : disdPal.text
              }
              TspinBox {
                from: 2; to: 32
                value: barNumber
                onValueModified: barNumber = value
              }
            }
          }
          RhythmDiversityTile {
            diversity: rhythmDiversity
            onDiversityModified: rhythmDiversity = diversity
          }
          EndOnTonicTile {
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            checked: endsOnTonic
            checkBox.onClicked: endsOnTonic = checked
          }
          CurrentKeyTile {
            checked: onlyCurrKey
            checkBox.onClicked: onlyCurrKey = checked
          }
          MaxIntervalTile {
            maxStep: melGenItem.maxStep
            onMaxModified: melGenItem.maxStep = maxStep
          }
        }
      }
    }
  }

  property var defButt: null
  Component.onCompleted: {
    dialLoader.title = Noo.TR("TmelodySettings", "Random melody")
    dialLoader.standardButtons = DialogButtonBox.Close | DialogButtonBox.Help | DialogButtonBox.RestoreDefaults
    defButt = dialLoader.buttonBox.standardButton(DialogButtonBox.RestoreDefaults)
    defButt.text = Noo.TR("TmainScoreObject", "Generate melody")
    defButt.DialogButtonBox.buttonRole = DialogButtonBox.ActionRole
    defButt.pixmap = Noo.pix("melody")
    defButt.clicked.connect(doGenerate)
  }

  function doGenerate() {
    generate()
    defButt.clicked.disconnect(doGenerate)
    dialLoader.close()
  }

  function help() {  console.log("'Melody generate' help is not implemented yet")   /*Noo.openHelpLink("")*/ }
}
