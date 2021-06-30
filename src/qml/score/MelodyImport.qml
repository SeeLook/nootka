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
  id: importWindow
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
      id: partList
      y: melTitle.height + NOO.factor() / 2
      width: parent.width; height: parent.height - melTitle.height - buttRow.height - NOO.factor()
      clip: true
      spacing: NOO.factor() / 2
      model: melImport.partsModel
      delegate: TipRect {
        x: NOO.factor() / 4
        width: melImport.width - NOO.factor() / 2; height: scoreCol.height + NOO.factor()
        Text {
          z: 5
          x: NOO.factor() * 6
          color: activPal.text
          text: qsTr("part") + ": " + modelData.part + ", " + qsTr("staff") + ": " + modelData.staff
          + ", " + qsTr("voice") + ": " + modelData.voice
        }
        Column {
          id: scoreCol
          width: parent.width
          z: 1
          Repeater {
            model: modelData.snippets
            Rectangle {
              width: parent.width; height: score.height + NOO.factor()
              color: NOO.alpha(activPal.highlight, importChB.checked ? 50 : 0)
              TcheckBox {
                id: importChB
                anchors.verticalCenter: parent.verticalCenter
                ButtonGroup.group: multiSelect ? null : group
              }
              Score {
                id: score
                y: NOO.factor() / 2; x: importChB.width + NOO.factor()
                width: melImport.width - importChB.width - NOO.factor() * 1.5; height: NOO.factor() * 14
                readOnly: true
                bgColor: "transparent"
              }
              Component.onCompleted: {
                modelData.setScoreObject(score.scoreObj)
                modelData.selected = Qt.binding(function() { return importChB.checked })
              }
            }
          }
        }
        Row { // single part actions for 'Divide' & 'Transpose'
          anchors { left: parent.left; top: parent.top; margins: NOO.factor() / 2 }
          RectButton {
            height: NOO.factor() * 2.2
            font { pixelSize: NOO.factor() * 2; family: "Nootka" }
            text: "\u2702"
            onClicked: {
              var p = parent.mapToItem(partList, NOO.factor() * 4, 0)
              dividePop.melPart = modelData
              dividePop.x = p.x
              dividePop.y = p.y
              dividePop.open()
            }
          }
          RectButton {
            height: NOO.factor() * 2.2
            font { pixelSize: NOO.factor() * 2; family: "Nootka" }
            text: "\u0192"
            onClicked: {
              var p = parent.mapToItem(partList, NOO.factor() * 4, 0)
              transPop.melPart = modelData
              transPop.x = p.x
              transPop.y = p.y
              transPop.open()
            }
          }
        }
      }
      ScrollBar.vertical: ScrollBar {}
    }

    Row { // Footer with 'Import' & 'Cancel' buttons
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

    Row { // Global 'Divide' & 'Transpose' actions
      anchors { right: parent.right; top: parent.top; topMargin: NOO.factor() / 4; rightMargin: NOO.factor() }
      RectButton {
        height: NOO.factor() * 3
        font { pixelSize: NOO.factor() * 2.8; family: "Nootka" }
        text: "\u2702"
        onClicked: {
          dividePop.melPart = null
          dividePop.x = (importWindow.width - dividePop.width) / 2
          dividePop.y = NOO.factor() * 2
          dividePop.open()
        }
      }
      RectButton {
        height: NOO.factor() * 3
        font { pixelSize: NOO.factor() * 2.8; family: "Nootka" }
        text: "\u0192"
        onClicked: {
          transPop.melPart = null
          transPop.x = (importWindow.width - transPop.width) / 2
          transPop.y = NOO.factor() * 2
          transPop.open()
        }
      }
    }
  }

  TpopupDialog {
    id: dividePop
    property var melPart: null
    width: divMel.width + NOO.factor() * 2
    height: divMel.height + NOO.factor() * (dividePop.melPart ? 5 : 7)
    caption: dividePop.melPart ? "" : qsTr("Transform all imported parts of the score")
    DivideMelody {
      id: divMel
    }
    onAccepted: {
      if (melPart)
        melPart.splitBarNr = divMel.divideBy
      else
        melImport.globalSplitNr = divMel.divideBy
    }
  }

  TpopupDialog {
    id: transPop
    property var melPart: null
    width: transpose.width + NOO.factor() * 2
    height: transpose.height + NOO.factor() * (transPop.melPart ? 5 : 7)
    caption: transPop.melPart ? "" : qsTr("Transform all imported parts of the score")
    Transpose {
      id: transpose
    }
  }

  onClosing: destroy()

}
