/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0

import "../"

Flickable {
  width: parent.width; height: parent.height
  clip: true
  contentHeight: accidsGrid.height + nootkaWindow.fontSize * 2
  contentWidth: width

  ScrollBar.vertical: ScrollBar { active: !Noo.isAndroid() }

  Grid {
    columns:  parent.width > nootkaWindow.fontSize * 60 ? 2 : 1
    id: accidsGrid
    width: parent.width
    spacing: nootkaWindow.fontSize / 4
    horizontalItemAlignment: Grid.AlignHCenter
    topPadding: nootkaWindow.fontSize

    Frame {
      width: accidsGrid.columns === 1 ? Math.max(parent.width * 0.9, dblAccidsChB.width) : parent.width * 0.35
      Column {
        spacing: nootkaWindow.fontSize / 2
        width: parent.width
        Text { text: qsTr("accidentals"); anchors.horizontalCenter: parent.horizontalCenter }
        Tile {
          CheckBox {
            id: sharpsChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("# - sharps")
          }
          description: qsTr("Sharps will be used in questions and answers.<br>It has to be checked, if keys with sharps are used.")
        }
        Tile {
          CheckBox {
            id: flatsChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("b - flats")
          }
          description: qsTr("Flats will be used in questions and answers.<br>It has to be checked, if keys with flats are used.")
        }
        Tile {
          CheckBox {
            id: dblAccidsChB
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("x, bb - double accidentals")
          }
        }
        Tile {
          CheckBox {
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
        spacing: nootkaWindow.fontSize / 2
        width: parent.width
        CheckBox {
          id: useKeysChB
          text: qsTr("use key signatures")
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
          }
          description: qsTr("random key signature from selected range.")
        }
        Tile {
          enabled: useKeysChB.checked
          Row {
            spacing: nootkaWindow.fontSize / 2
            anchors.horizontalCenter: parent.horizontalCenter
            ComboBox {
              id: fromCombo
              model: Noo.keyComboModel()
              delegate: ItemDelegate { text: modelData }
              width: nootkaWindow.fontSize * 15
              currentIndex: 7 // C-major
            }
            Rectangle {
              color: enabled ? activPal.text : disdPal.text; width: nootkaWindow.fontSize; height: nootkaWindow.fontSize / 5
              anchors.verticalCenter: parent.verticalCenter
            }
            ComboBox {
              id: toCombo
              model: Noo.keyComboModel()
              delegate: ItemDelegate { text: modelData }
              width: nootkaWindow.fontSize * 15
              currentIndex: 7
            }
          }
          description: qsTr("Select a key signature. Appropriate accidentals used in the level will be selected automatically.")
        }
        Tile {
          enabled: useKeysChB.checked
          CheckBox {
            id: keyInAnswersChB
            text: qsTr("select a key signature manually")
            anchors.horizontalCenter: parent.horizontalCenter
          }
          description: qsTr("if checked, in exam user have to select a key signature, otherwise it is shown by application.")
        }
        Tile {
          CheckBox {
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
