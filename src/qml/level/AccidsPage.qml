/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tflickable {
  height: parent.height
  contentHeight: accidsGrid.height + Noo.fontSize() * 2
  contentWidth: Math.max(width, Noo.fontSize() * 40)

  Grid {
    columns:  parent.width > Noo.fontSize() * 60 ? 2 : 1
    id: accidsGrid
    width: parent.width
    spacing: Noo.fontSize() / 4
    horizontalItemAlignment: Grid.AlignHCenter
    topPadding: Noo.fontSize()

    Tframe {
      width: accidsGrid.columns === 1 ? Math.max(parent.width * 0.9, dblAccidsChB.width) : parent.width * 0.4
      Column {
        spacing: Noo.fontSize() / 2
        width: parent.width
        Text { text: qsTr("accidentals"); color: activPal.text; anchors.horizontalCenter: parent.horizontalCenter }
        Tile {
          TcheckBox {
            id: sharpsChB
            enabled: !(useKeysChB.checked && (loKeyCombo.currentIndex > 7 || (rangeKeysChB.checked && hiKeyCombo.currentIndex > 7)))
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("# - sharps")
            checked: creator.withSharps
            onClicked: creator.withSharps = checked
          }
          description: qsTr("Sharps will be used in questions and answers. It has to be checked, if keys with sharps are used.")
        }
        Tile {
          TcheckBox {
            id: flatsChB
            enabled: !(useKeysChB.checked && (loKeyCombo.currentIndex < 7 || (rangeKeysChB.checked && hiKeyCombo.currentIndex < 7)))
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("b - flats")
            checked: creator.withFlats
            onClicked: creator.withFlats = checked
          }
          description: qsTr("Flats will be used in questions and answers. It has to be checked, if keys with flats are used.")
        }
        Tile {
          TcheckBox {
            id: dblAccidsChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("x, bb - double accidentals")
            checked: creator.withDblAccids
            onClicked: creator.withDblAccids = checked
          }
        }
        Tile {
          TcheckBox {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("force using appropriate accidental")
            checked: creator.forceAccids
            onClicked: creator.forceAccids = checked
          }
          description: qsTr("if checked, it is possible to select a note with given accidental only.")
        }
      }
    } // accids frame

    Tframe {
      id: keyFrame
      width: accidsGrid.columns === 1 ? parent.width * 0.98 : parent.width * 0.59
      Column {
        spacing: Noo.fontSize() / 2
        width: parent.width
        TcheckBox {
          id: useKeysChB
          text: Noo.TR("ScorePage", "use key signatures")
          anchors.horizontalCenter: parent.horizontalCenter
          checked: creator.useKeySign
          onClicked: creator.useKeySign = checked
        }
        ButtonGroup { id: keysGr }
        Tile {
          enabled: useKeysChB.checked
          TradioButton {
            text: qsTr("single key")
            ButtonGroup.group: keysGr
            anchors.horizontalCenter: parent.horizontalCenter
            checked: creator.isSingleKey
            onClicked: creator.isSingleKey = checked
          }
          description: qsTr("Only one, selected key signature for whole exam.")
        }
        Tile {
          enabled: useKeysChB.checked
          TradioButton {
            id: rangeKeysChB
            text: qsTr("range of keys")
            ButtonGroup.group: keysGr
            anchors.horizontalCenter: parent.horizontalCenter
            checked: !creator.isSingleKey
            onClicked: creator.isSingleKey = !checked
          }
          description: qsTr("Random key signature from selected range.")
        }
        Tile {
          enabled: useKeysChB.checked
          Row {
            spacing: Noo.fontSize() / 2
            anchors.horizontalCenter: parent.horizontalCenter
            TcomboBox {
              id: loKeyCombo
              model: creator.keyComboModel()
              width: keyFrame.width * 0.4
              currentIndex: creator.loKey + 7
              onActivated: creator.loKey = currentIndex - 7
            }
            Rectangle {
              color: enabled ? activPal.text : disdPal.text; width: Noo.fontSize(); height: Noo.fontSize() / 5
              anchors.verticalCenter: parent.verticalCenter
            }
            TcomboBox {
              enabled: rangeKeysChB.checked
              id: hiKeyCombo
              model: creator.keyComboModel()
              width: keyFrame.width * 0.4
              currentIndex: creator.hiKey + 7
              onActivated: creator.hiKey = currentIndex - 7
            }
          }
          description: qsTr("Select a key signature. Appropriate accidentals used in this level will be selected automatically.")
        }
        Tile {
          enabled: useKeysChB.checked
          TcheckBox {
            text: qsTr("select a key signature manually")
            anchors.horizontalCenter: parent.horizontalCenter
            checked: creator.manualKey
            onClicked: creator.manualKey = checked
          }
          description: qsTr("if checked, in exam user have to select a key signature, otherwise it is shown by application.")
        }
        CurrentKeyTile {
          checked: creator.onlyCurrKey
          checkBox.onClicked: creator.onlyCurrKey = checked
          description: qsTr("Only notes from current key signature are taken. If key signature is disabled accidentals are not used.")
        }
      }
    } // keyFrame

  }
}
