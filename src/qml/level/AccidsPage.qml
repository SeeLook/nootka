/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
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

    Frame {
      width: accidsGrid.columns === 1 ? Math.max(parent.width * 0.9, dblAccidsChB.width) : parent.width * 0.35
      Column {
        spacing: Noo.fontSize() / 2
        width: parent.width
        Text { text: qsTr("accidentals"); color: activPal.text; anchors.horizontalCenter: parent.horizontalCenter }
        Tile {
          TcheckBox {
            id: sharpsChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("# - sharps")
          }
          description: qsTr("Sharps will be used in questions and answers.<br>It has to be checked, if keys with sharps are used.")
        }
        Tile {
          TcheckBox {
            id: flatsChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("b - flats")
          }
          description: qsTr("Flats will be used in questions and answers.<br>It has to be checked, if keys with flats are used.")
        }
        Tile {
          TcheckBox {
            id: dblAccidsChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("x, bb - double accidentals")
          }
        }
        Tile {
          TcheckBox {
            id: forceAccChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("force using appropriate accidental")
          }
          description: qsTr("if checked, it is possible to select a note<br>with given accidental only.")
        }
      }
    }

    Frame {
      width: accidsGrid.columns === 1 ? parent.width * 0.96 : parent.width * 0.64
      Column {
        spacing: Noo.fontSize() / 2
        width: parent.width
        TcheckBox {
          id: useKeysChB
          text: Noo.TR("TscoreSettings", "enable key signature")
          anchors.horizontalCenter: parent.horizontalCenter
          checked: true
        }
        ButtonGroup { id: keysGr }
        Tile {
          enabled: useKeysChB.checked
          RadioButton {
            text: qsTr("single key")
            ButtonGroup.group: keysGr
            anchors.horizontalCenter: parent.horizontalCenter
          }
          description: qsTr("only one, selected key signature for whole exam.")
        }
        Tile {
          enabled: useKeysChB.checked
          RadioButton {
            text: qsTr("range of keys")
            ButtonGroup.group: keysGr
            anchors.horizontalCenter: parent.horizontalCenter
            checked: true
          }
          description: qsTr("random key signature from selected range.")
        }
        Tile {
          enabled: useKeysChB.checked
          Row {
            spacing: Noo.fontSize() / 2
            anchors.horizontalCenter: parent.horizontalCenter
            ComboBox {
              id: fromCombo
              model: Noo.keyComboModel()
              delegate: ItemDelegate { text: modelData }
              width: Noo.fontSize() * 15
              currentIndex: 7 // C-major
            }
            Rectangle {
              color: enabled ? activPal.text : disdPal.text; width: Noo.fontSize(); height: Noo.fontSize() / 5
              anchors.verticalCenter: parent.verticalCenter
            }
            ComboBox {
              id: toCombo
              model: Noo.keyComboModel()
              delegate: ItemDelegate { text: modelData }
              width: Noo.fontSize() * 15
              currentIndex: 7
            }
          }
          description: qsTr("Select a key signature. Appropriate accidentals used in this level will be selected automatically.")
        }
        Tile {
          enabled: useKeysChB.checked
          TcheckBox {
            id: keyInAnswersChB
            text: qsTr("select a key signature manually")
            anchors.horizontalCenter: parent.horizontalCenter
          }
          description: qsTr("if checked, in exam user have to select a key signature, otherwise it is shown by application.")
        }
        Tile {
          TcheckBox {
            id: currKeyOnlyChB
            text: qsTr("notes in current key signature only")
            anchors.horizontalCenter: parent.horizontalCenter
          }
          description: qsTr("Only notes from current key signature are taken. If key signature is disabled accidentals are not used.")
        }
      }
    }

  }
}
