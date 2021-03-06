/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
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
  property alias multiSelect: melImport.multiSelect

  visibility: NOO.isAndroid() && GLOB.fullScreen() ? "FullScreen" : "AutomaticVisibility"
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
      font { pixelSize: NOO.factor() * (NOO.isAndroid() ? 1 : 1.2); bold: true }
      text: melImport.title
    }
    ListView {
      id: partList
      y: melTitle.height + NOO.factor() * (NOO.isAndroid() ? 1.1 : 2)
      width: parent.width; height: parent.height - melTitle.height - buttRow.height - NOO.factor() * 2.5
      clip: true
      spacing: NOO.factor() / (NOO.isAndroid() ? 4 : 2)
      model: melImport.partsModel
      delegate: TipRect {
        x: NOO.factor() / 4
        width: melImport.width - NOO.factor() / 2; height: scoreCol.height + NOO.factor()
        Text {
          z: 5
          x: NOO.factor() * 2
          color: activPal.text
          text: modelData.partName
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
                width: melImport.width - importChB.width - NOO.factor() * 1.5
                height: NOO.isAndroid() ? Math.min(NOO.factor() * 8, importWindow.height / 3) : NOO.factor() * 15
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
          anchors { right: parent.right; top: parent.top; margins: NOO.factor() / 2 }
          z: 7
          RectButton {
            height: NOO.factor() * (NOO.isAndroid() ? 1.8 : 2.2)
            font { pixelSize: NOO.factor() * (NOO.isAndroid() ? 1.5 : 2); family: "Nootka" }
            text: "\u2702"
            onClicked: {
              if (!dividePop)
                dividePop = divideComp.createObject(importWindow)
              var p = parent.mapToItem(partList, 0, (NOO.factor() * 15 - dividePop.height) / 2)
              dividePop.melPart = modelData
              dividePop.x = partList.width - dividePop.width - NOO.factor() * 4
              dividePop.y = NOO.bound(0, p.y, partList.height - dividePop.height)
              dividePop.open()
            }
          }
          RectButton {
            height: NOO.factor() * (NOO.isAndroid() ? 1.8 : 2.2)
            font { pixelSize: NOO.factor() * (NOO.isAndroid() ? 1.5 : 2); family: "Nootka" }
            text: "\u0192"
            onClicked: {
              if (!transPop)
                transPop = transComp.createObject(importWindow)
              var p = parent.mapToItem(partList, 0, 0)
              transPop.melPart = modelData
              transPop.initialKey = modelData.key
              transPop.x = partList.width - transPop.width - NOO.factor() * 4
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
        color: Qt.tint(activPal.button, NOO.alpha("red", NOO.isAndroid() ? 40 : 0))
        onClicked: close()
      }
      TiconButton {
        y: NOO.factor() / 4
        pixmap: NOO.pix("melody")
        text: qsTr("Import")
        color: Qt.tint(activPal.button, NOO.alpha("teal", NOO.isAndroid() ? 40 : 0))
        onClicked: {
          melImport.emitImport()
          close()
        }
      }
    }

    Row { // Global 'Divide' action
      anchors { left: parent.left; top: parent.top; topMargin: NOO.factor() / 4; leftMargin: NOO.factor() / 2 }
      RectButton {
        height: NOO.factor() * (NOO.isAndroid() ? 2 : 3)
        font { pixelSize: NOO.factor() * (NOO.isAndroid() ? 1.5 : 2.8); family: "Nootka" }
        text: "\u2702"
        onClicked: {
          if (!dividePop)
            dividePop = divideComp.createObject(importWindow)
          dividePop.melPart = null
          dividePop.x = (importWindow.width - dividePop.width) / 2
          dividePop.y = NOO.factor() * 2
          dividePop.open()
        }
      }
    }
  }

  Component.onCompleted: {
    if (GLOB.gotIt("ScoreImport", true))
      Qt.createComponent("qrc:/gotit/ImportInfo.qml").createObject(importWindow, { "remaindChecked": true })
  }

  property var dividePop: null
  Component {
    id: divideComp
    TpopupDialog {
      property var melPart: null
      width: divMel.width + NOO.factor() * 2
      height: divMel.height + NOO.factor() * 7
      caption: melPart ? "" : qsTr("Transform all parts of the score")
      DivideMelody {
        id: divMel
        divisionBy: parent.melPart ? dividePop.melPart.splitBarNr : melImport.globalSplitNr
      }
      onAccepted: {
        if (melPart)
          melPart.splitBarNr = divMel.divisionBy
        else
          melImport.globalSplitNr = divMel.divisionBy
      }
    }
  }

  property var transPop: null
  Component {
    id: transComp
    TpopupDialog {
      property var melPart: null
      property alias initialKey: transpose.initialKey
      width: transpose.width + NOO.factor() * 2
      height: transpose.height + NOO.factor() * (melPart ? 5 : 7)
      caption: melPart ? "" : qsTr("Transform all parts of the score")
      Transpose {
        id: transpose
      }
      onAboutToShow: {
        transpose.toKey = false
        transpose.byInterval = false
      }
      onAccepted: {
        if (transpose.toKey || transpose.byInterval) {
          if (transpose.toKey && melPart)
            transPop.melPart.key = transpose.selectedKey
          melImport.transpose(transpose.outShift, transpose.outScaleToRest, transpose.inInstrumentScale, transPop.melPart)
        }
      }
    }
  }

  property var chordView: null
  Component {
    id: chordComp
    MelodyPreview { // preview of chord notes
      property var chordIt: null
      melody: chordIt ? chordIt.chord : null
      showButtons: false
      width: NOO.factor() * 20 ; maxHeight: NOO.factor() * 22
      caption: qsTr("Select single note")
      selectReadOnly: true
      onReadOnlyNoteClicked: {
        hi.parent = score.note(noteId)
        chordIt.selected = noteId
      }
      onAboutToShow: {
        reload()
        hi.parent = score.note(chordIt.selected)
        var p = parent.mapFromItem(chordIt, 0, chordIt.height / 2)
        x = p.x - (p.x > parent.width / 2 ? width + NOO.factor() : - NOO.factor() * 2)
        y = NOO.bound(NOO.factor(), p.y - height / 2, parent.height - height - NOO.factor())
      }
      Rectangle { // selected chord note highlight
        id: hi
        parent: null
        visible: parent != null
        width: parent ? (parent.width - parent.alterWidth) * 1.5 : 0
        height: parent ? Math.min(12.0, parent.notePosY + 6.0) : 0
        x: parent ? -width * 0.25 : 0
        y: parent ? Math.min(parent.height - height, Math.max(0.0, parent.notePosY - height / 2.0)) : 0
        color: NOO.alpha(activPal.highlight, 75)
        z: -1
        radius: width / 3.0
      }
    }
  }

  function showChord(chordIt) {
    if (!chordView)
      chordView = chordComp.createObject(importWindow)
    chordView.chordIt = chordIt
    chordView.open()
  }

  onClosing: destroy()

}
