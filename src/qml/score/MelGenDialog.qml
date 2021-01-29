/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
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
      leftPadding: Noo.factor() / 2
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
          width: melGenItem.width - rtmSel.width * 1.2 - Noo.factor()
          Tile {
            Row {
              id: measRow
              anchors.horizontalCenter: parent.horizontalCenter
              spacing: Noo.factor()
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

  property var genButt: null

  Component.onCompleted: {
    dialLoader.title = Noo.TR("TmelodySettings", "Random melody")
    dialLoader.standardButtons = DialogButtonBox.Close | DialogButtonBox.Help | DialogButtonBox.RestoreDefaults
    genButt = dialLoader.buttonBox.standardButton(DialogButtonBox.RestoreDefaults)
    genButt.text = Noo.TR("TmainScoreObject", "Generate melody")
    genButt.DialogButtonBox.buttonRole = DialogButtonBox.ActionRole
    genButt.pixmap = Noo.pix("melody")
  }

  Connections {
    target: genButt
    onClicked: {
      generate()
      dialLoader.close()
    }
  }

  function help() {  console.log("'Melody generate' help is not implemented yet")   /*Noo.openHelpLink("")*/ }
}
