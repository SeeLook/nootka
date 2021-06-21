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
  visible: true
  modality: Qt.WindowModal
  title: qsTr("Import melody")
  width: nootkaWindow.width; height: nootkaWindow.height; x: nootkaWindow.x; y: nootkaWindow.y
  color: activPal.window

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
      width: parent.width; height: parent.height - melTitle.height - NOO.factor()
      clip: true
      spacing: 1
      model: melImport.partsModel
      delegate: Rectangle {
        width: melImport.width; height: NOO.factor() * 11
        color: index % 2 ? activPal.base : activPal.alternateBase
        Text {
          x: NOO.factor()
          color: activPal.text
          text: qsTr("part") + ": " + modelData.part + ", " + qsTr("staff") + ": " + modelData.staff
                + ", " + qsTr("voice") + ": " + modelData.voice
        }
        Score {
          id: score
          y: NOO.factor()
          width: parent.width; height: parent.height - NOO.factor()
          readOnly: true
        }
        Component.onCompleted: {
          modelData.setScoreObject(score.scoreObj)
        }
      }
    }
  }

}
