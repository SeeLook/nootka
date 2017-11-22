/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Dialogs 1.0
import "../"


TlevelsSelector {
  id: lSelector

  levelPreview: previewItem

  Row {
    width: parent.width
    spacing: width / 100
    padding: width / 200

    Column {
      width: parent.width * 0.49; height: lSelector.height
      spacing: lSelector.width / 100

      ListView {
        id: view
        clip: true
        width: parent.width; height: lSelector.height * 0.96 - buttRow.height
        currentIndex: -1
        ScrollBar.vertical: ScrollBar { active: false; visible: active }
        model: levelsModel
        delegate: Component {
          Item {
            width: view.width; height: delegateRow.height
            Rectangle {
              anchors.fill: parent
              color: index === view.currentIndex ? activPal.highlight :
                  (area.containsMouse ? Qt.tint(activPal.base, Noo.alpha(activPal.highlight, 50)) : (index % 2 === 1 ? activPal.alternateBase : activPal.base))
            }
            Row {
              id: delegateRow
              width: parent.width
              padding: width / 100
              Text {
                anchors.verticalCenter: parent.verticalCenter
                font { family: "Nootka"; pixelSize: Noo.fontSize() * 1.7 }
                width: Noo.fontSize() * 2.5
                color: index === view.currentIndex ? activPal.highlightedText : activPal.text
                text: isMelody(index) ? "m" : "n"
                horizontalAlignment: Text.AlignHCenter
              }
              Column {
                width: parent.width - Noo.fontSize() * 3
                spacing: Noo.fontSize() / 4
                anchors.verticalCenter: parent.verticalCenter
                Text {
                  width: parent.width
                  font { pixelSize: Noo.fontSize(); bold: true }
                  text: modelData
                  color: index === view.currentIndex ? activPal.highlightedText : activPal.text
                  elide: Text.ElideRight
                }
                Text {
                  visible: text !== ""
                  width: parent.width
                  text: desc(index)
                  font.pixelSize: Noo.fontSize() * 0.8
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
          }
        }
      }

      Row {
        id: buttRow
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: lSelector.width / 50

        TcuteButton {
          contentItem: Row {
            padding: lSelector.width / 200
            spacing: lSelector.width / 100
            Image {
              source: Noo.pix("nootka-level")
              sourceSize.height: Noo.fontSize() * 2
            }
            Text {
              anchors.verticalCenter: parent.verticalCenter
              color: enabled ? (checked ? activPal.highlightedText : activPal.text) : disdPal.text
              text: qsTr("Load")
            }
          }
          onClicked: loadFromFile()
        }
        TcuteButton {
          enabled: isRemovable(view.currentIndex)
          contentItem: Row {
            padding: lSelector.width / 200
            spacing: lSelector.width / 100
            Image {
              source: Noo.pix("delete")
              sourceSize.height: Noo.fontSize() * 2
            }
            Text {
              anchors.verticalCenter: parent.verticalCenter
              color: enabled ? (checked ? activPal.highlightedText : activPal.text) : disdPal.text
              text: qsTr("Remove")
            }
          }
          onClicked: {
            var c = Qt.createComponent("qrc:/level/RemoveLevel.qml")
            var rmLevelDialog = c.createObject(lSelector, { "levelName": levelName(view.currentIndex), "levelFile": levelFile(view.currentIndex) })
            rmLevelDialog.onRemove.connect(rmLevelSlot)
          }
        }
      }
    }

    LevelPreview {
      id: previewItem
      width: parent.width * 0.49; height: lSelector.height
    }
  }

  function rmLevelSlot(fromDisk) {
    if (removeLevel(view.currentIndex, fromDisk))
      view.currentIndex = -1
  }
}
