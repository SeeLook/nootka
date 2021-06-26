/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import Nootka.Dialogs 1.0
import Score 1.0
import "../"
import "../score"


Window {
  property bool multiSelect: false

  visible: true
  modality: Qt.WindowModal
  title: qsTr("Import melody")
  width: nootkaWindow.width; height: nootkaWindow.height; x: nootkaWindow.x; y: nootkaWindow.y
  color: activPal.window

  ButtonGroup { id: group }

  TmelodyImportItem {
    id: melImport
    anchors.fill: parent
    Text {
      id: melTitle
      anchors.horizontalCenter: parent.horizontalCenter
      color: activPal.text
      font { pixelSize: NOO.factor() * 1.2; bold: true }
      text: melImport.title
    }
    ListView {
      y: melTitle.height + NOO.factor() / 2
      width: parent.width; height: parent.height - melTitle.height - buttRow.height - NOO.factor()
      clip: true
      spacing: NOO.factor() / 2
      model: melImport.partsModel
      delegate: TipRect {
        x: NOO.factor() / 4
        width: melImport.width - NOO.factor() / 2; height: scoreCol.height + NOO.factor()
        color: index % 2 ? activPal.base : activPal.alternateBase
        Text {
          z: 5
          x: NOO.factor() * 3
          color: activPal.text
          text: qsTr("part") + ": " + modelData.part + ", " + qsTr("staff") + ": " + modelData.staff
          + ", " + qsTr("voice") + ": " + modelData.voice
        }
        Column {
          id: scoreCol
          z: 1
          Repeater {
            model: modelData.snippets
            Row {
              TcheckBox {
                id: importChB
                anchors.verticalCenter: parent.verticalCenter
                ButtonGroup.group: multiSelect ? null : group
              }
              Score {
                id: score
                y: NOO.factor()
                width: melImport.width - importChB.width - NOO.factor() * 1.5; height: NOO.factor() * 14
                readOnly: true
                bgColor: NOO.alpha(activPal.highlight, importChB.checked ? 50 : 0)
              }
              Component.onCompleted: {
                modelData.setScoreObject(score.scoreObj)
                modelData.selected = Qt.binding(function() { return importChB.checked })
              }
            }
          }
        }
      }
      ScrollBar.vertical: ScrollBar {}
    }
    Row {
      id: buttRow
      y: parent.height - height - NOO.factor() / 2; x: (parent.width - width) / 2
      spacing: NOO.factor() * 2
      TiconButton {
        y: NOO.factor() / 4
        pixmap: NOO.pix("exit")
        text: NOO.TR("QShortcut", "Cancel")
        onClicked: close()
      }
      TiconButton {
        y: NOO.factor() / 4
        pixmap: NOO.pix("melody")
        text: qsTr("Import")
        onClicked: {
          melImport.emitImport()
          close()
        }
      }
    }
  }

  TcuteButton {
    anchors { right: parent.right; top: parent.top; margins: NOO.factor() / 2 }
    height: NOO.factor() * 3
    font { pixelSize: NOO.factor() * 2; bold: true }
    text: "⋮"
    onClicked: settPop.open()
  }

  TpopupDialog {
    id: settPop
    width: popCol.width + NOO.factor() * 2; height: popCol.height + NOO.factor() * 5
    Column {
      id: popCol
      Row {
        spacing: NOO.factor()
        Text {
          anchors.verticalCenter: parent.verticalCenter
          color: activPal.text; text: qsTr("Split after every bar number")
        }
        TspinBox {
          id: splitSpin
          from: 0; to: 64
          value: melImport.globalSplitNr
        }
      }
    }
    onAccepted: melImport.globalSplitNr = splitSpin.value
  }

  onClosing: destroy()

}
