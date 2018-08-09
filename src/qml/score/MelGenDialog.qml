/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
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
            anchors.horizontalCenter: parent.horizontalCenter
            id: rtmSel
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
              SpinBox {
                editable: true
                from: 2; to: 32
                value: barNumber
                onValueModified: barNumber = value
              }
            }
          }
          Tile {
            description: qsTr("Smaller values - less number of selected rhythmic note groups will be used, biggest value - melody will consist maximal possible number of selected groups.")
            Row {
              id: diverRow
              anchors.horizontalCenter: parent.horizontalCenter
              spacing: Noo.fontSize()
              Text {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Rhythmic diversity")
                color: enabled ? activPal.text : disdPal.text
              }
              SpinBox {
                id: rtmDivSpin
                editable: true
                from: 1; to: 10
                value: rhythmDiversity
                onValueModified: rhythmDiversity = value
              }
            }
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

  Component.onCompleted: {
    if (Noo.isAndroid()) {
        dialLoader.buttons = [DialogButtonBox.Close, DialogButtonBox.Help, DialogButtonBox.RestoreDefaults]
    } else {
        dialLoader.title = Noo.TR("TmelodySettings", "Random melody")
        dialLoader.standardButtons = DialogButtonBox.Close | DialogButtonBox.Help | DialogButtonBox.RestoreDefaults
        var b = dialLoader.buttonBox.standardButton(DialogButtonBox.RestoreDefaults)
        b.text = Noo.TR("TmelMan", "Generate melody")
        b.DialogButtonBox.buttonRole = DialogButtonBox.ActionRole
        b.pixmap = Noo.pix("melody")
        b.clicked.connect(doGenerate)
    }
  }

  function doGenerate() {
    generate()
    dialLoader.close()
  }

  function help() {  console.log("'Melody generate' help is not implemented yet")   /*Noo.openHelpLink("")*/ }
}
