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
        TcuteButton {
          anchors { left: parent.left; top: parent.top; margins: NOO.factor() / 2 }
          height: NOO.factor() * 2.5
          font { pixelSize: NOO.factor() * 1.5; bold: true }
          text: "⋮"
          onClicked: {
            var p = parent.mapToItem(partList, NOO.factor() * 4, 0)
            settPop.melPart = modelData
            settPop.x = p.x
            settPop.y = p.y
            settPop.open()
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
    onClicked: {
      settPop.melPart = null
      settPop.x = (importWindow.width - settPop.width) / 2
      settPop.y = NOO.factor()
      settPop.open()
    }
  }

  TpopupDialog {
    id: settPop
    property var melPart: null
    width: popCol.width + NOO.factor() * 2; height: popCol.height + NOO.factor() * 5
    Column {
      id: popCol
      spacing: NOO.factor() / 2
      Text {
        visible: !settPop.melPart
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.text; font.bold: true
        text: qsTr("Transform all imported parts of the score.")
      }
      Tile {
        width: splitRow.width + NOO.factor() * 3
        Row {
          id: splitRow
          anchors.horizontalCenter: parent.horizontalCenter
          spacing: NOO.factor()
          Text {
            anchors.verticalCenter: parent.verticalCenter
            color: activPal.text; text: qsTr("Divide by selected bars number")
          }
          TspinBox {
            id: splitSpin
            from: 0; to: 64
            value: settPop.melPart ? settPop.melPart.splitBarNr : melImport.globalSplitNr
          }
        }
        description: "<br>" + qsTr("No division if set to 0.")
      }
    }
    onAccepted: {
      if (melPart)
        melPart.splitBarNr = splitSpin.value
      else
        melImport.globalSplitNr = splitSpin.value
    }
  }

  onClosing: destroy()

}
