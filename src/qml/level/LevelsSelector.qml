/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Dialogs 1.0
import "../"


TlevelsSelector {
  id: lSelector

  property alias levelId: view.currentIndex

  levelPreview: previewItem
  currentIndex: view.currentIndex

  onSelectLast: view.currentIndex = view.count - 1

  // private
  property bool isVertical: parent.height * 1.2 > parent.width

  Grid {
    width: parent.width
    columns: isVertical ? 1 : 2
    spacing: width / 100
    padding: width / 200

    Column {
      width: lSelector.width * (isVertical ? 1 : 0.49 )
      spacing: Math.min(lSelector.width, lSelector.height) / 100

      ListView {
        id: view
        clip: true
        width: parent.width
        height: lSelector.height * (isVertical ? 0.48 : 0.96) - buttRow.height
        spacing: 1
        currentIndex: -1
        ScrollBar.vertical: ScrollBar { active: false; visible: active }
        model: levelsModel
        delegate: Rectangle {
          width: view.width; height: delegateRow.height
          color: index === view.currentIndex ? activPal.highlight :
              (area.containsMouse ? Qt.tint(activPal.base, NOO.alpha(activPal.highlight, 50)) : (index % 2 === 1 ? activPal.alternateBase : activPal.base))
          Row {
            id: delegateRow
            width: parent.width
            padding: width / 100
            Text {
              anchors.verticalCenter: parent.verticalCenter
              font { family: "Nootka"; pixelSize: NOO.factor() * 1.7 }
              width: NOO.factor() * 2.5
              color: index === view.currentIndex ? activPal.highlightedText : isMelody(index) ? NOO.invert(activPal.highlight) : activPal.highlight
              text: isMelody(index) ? "m" : "n"
              horizontalAlignment: Text.AlignHCenter
            }
            Column {
              width: parent.width - NOO.factor() * 3
              spacing: NOO.factor() / 4
              anchors.verticalCenter: parent.verticalCenter
              Text {
                width: parent.width
                font { pixelSize: NOO.factor(); bold: true }
                text: modelData
                color: index === view.currentIndex ? activPal.highlightedText : (isSuitable(index) ? activPal.text : disdPal.text)
                elide: Text.ElideRight
              }
              Text {
                visible: text !== ""
                width: parent.width
                text: desc(index)
                font.pixelSize: NOO.factor() * 0.8
                color: index === view.currentIndex ? activPal.highlightedText : activPal.text
                wrapMode: Text.WordWrap; maximumLineCount: 2; elide: Text.ElideRight
              }
            }
          }
          MouseArea {
            anchors.fill: parent
            id: area
            hoverEnabled: true
            onClicked: {
              view.currentIndex = index
              showLevel(index)
            }
          }
        } // Rectangle
      }

      Row {
        id: buttRow
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: lSelector.width / 50

        TiconButton {
          pixmap: NOO.pix("nootka-level"); text: qsTr("Load")
          onClicked: loadFromFile()
        }
        TiconButton {
          enabled: isRemovable(view.currentIndex)
          pixmap: NOO.pix("delete"); text: NOO.TR("QFileDialog", "Remove")
          onClicked: {
            var c = Qt.createComponent("qrc:/level/RemoveLevel.qml")
            var rmLevelDialog = c.createObject(lSelector, { "levelName": levelName(view.currentIndex), "levelFile": levelFile(view.currentIndex) })
            rmLevelDialog.onRemove.connect(rmLevelSlot)
          }
        }
      }
    } // Column of level list

    LevelPreview {
      id: previewItem
      width: lSelector.width * (isVertical ? 1 : 0.49 )
      height: lSelector.height * (isVertical ? 0.49 : 1)
    }
  }

  function rmLevelSlot(fromDisk) {
    if (removeLevel(view.currentIndex, fromDisk))
      view.currentIndex = -1
  }

  onWarnMessage: {
    Qt.createComponent("qrc:/Tmessage.qml").createObject(lSelector.parent, { "message": message, "accent": accent })
  }
}
