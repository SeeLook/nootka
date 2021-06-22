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
      spacing: 1
      model: melImport.partsModel
      delegate: Rectangle {
        width: melImport.width; height: NOO.factor() * 15
        color: importChB.checked ? Qt.tint(activPal.base, NOO.alpha(activPal.highlight, 50)) : (index % 2 ? activPal.base : activPal.alternateBase)
        TcheckBox {
          id: importChB
          anchors.verticalCenter: parent.verticalCenter
          ButtonGroup.group: multiSelect ? null : group
        }
        Text {
          x: importChB.width + NOO.factor()
          color: activPal.text
          text: qsTr("part") + ": " + modelData.part + ", " + qsTr("staff") + ": " + modelData.staff
                + ", " + qsTr("voice") + ": " + modelData.voice
        }
        Score {
          id: score
          y: NOO.factor(); x: importChB.width
          width: parent.width - importChB.width; height: parent.height - NOO.factor()
          readOnly: true
        }
        Component.onCompleted: {
          modelData.setScoreObject(score.scoreObj)
          modelData.selected = Qt.binding(function() { return importChB.checked })
        }
      }
    }
    Row {
      id: buttRow
      y: parent.height - height - NOO.factor() / 2; x: (parent.width - width) / 2
      spacing: NOO.factor() * 2
      TiconButton {
        pixmap: NOO.pix("exit")
        text: NOO.TR("QShortcut", "Cancel")
        onClicked: close()
      }
      TiconButton {
        pixmap: NOO.pix("melody")
        text: qsTr("Import")
        onClicked: {
          melImport.emitImport()
          close()
        }
      }
    }
  }

  onClosing: destroy()

}
