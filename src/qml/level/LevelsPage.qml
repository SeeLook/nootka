/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


Column {
  id: levelsPage

  height: parent.height; width: parent.width

  spacing: levelsPage.width / 50
  LevelsSelector {
    id: selector
    width: parent.width; height: levelsPage.height - buttRow.height - levelsPage.width / 50
    Component.onCompleted: creator.selector = selector
  }
  Row {
    id: buttRow
    spacing: levelsPage.width / 50
    anchors.horizontalCenter: parent.horizontalCenter
    TcuteButton {
      enabled: creator.notSaved
      width: levelsPage.width / 4
      contentItem: Row {
        padding: levelsPage.width / 200
        spacing: levelsPage.width / 100
        Image {
          source: Noo.pix("notSaved")
          sourceSize.height: levelsPage.height / 15
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          color: enabled ? (checked ? activPal.highlightedText : activPal.text) : disdPal.text
          text: qsTr("Save")
        }
      }
      onClicked: {
        creator.saveLevel()
      }
    }
    TcuteButton {
      enabled: selector.levelId !== -1 && selector.isSuitable(selector.levelId)
      width: levelsPage.width / 4
      contentItem: Row {
        padding: levelsPage.width / 200
        spacing: levelsPage.width / 100
        Image {
          source: Noo.pix("exam")
          sourceSize.height: levelsPage.height / 15
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          color: enabled ? (checked ? activPal.highlightedText : activPal.text) : disdPal.text
          text: qsTr("Start exam")
        }
      }
      onClicked: {
        
      }
    }
    TcuteButton {
      enabled: selector.levelId !== -1 && selector.isSuitable(selector.levelId)
      width: levelsPage.width / 4
      contentItem: Row {
        padding: levelsPage.width / 200
        spacing: levelsPage.width / 100
        Image {
          source: Noo.pix("practice")
          sourceSize.height: levelsPage.height / 15
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          color: enabled ? (checked ? activPal.highlightedText : activPal.text) : disdPal.text
          text: qsTr("Start exercise")
        }
      }
      onClicked: {
        
      }
    }
  }
}
