/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tflickable {
  id: root
  contentHeight: donCol.height
  z: 1

  Rectangle {
    id: bgRect
    width: root.width; height: root.height
    color: Noo.alpha(activPal.base, 230)
    parent: root.parent
    z: 0
    Image {
      source: Noo.pix("nootka")
      height: root.height; width: height
      z: -1
      anchors.horizontalCenter: parent.horizontalCenter
    }
  }

  Column {
    id: donCol
    width: root.width
    spacing: Noo.fontSize()

    Tile {
      width: parent.width
      anchors.horizontalCenter: undefined
      bgColor: activPal.highlight
      Text {
        text: qsTranslate("TaboutNootka", "People and companies who gave material support for the Nootka project")
        width: parent.width * 0.9
        wrapMode: Text.WordWrap
        font { pixelSize: Noo.fontSize() * 2; bold: true }
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.highlightedText
        horizontalAlignment: Text.AlignHCenter
      }
    }

    Grid {
      id: container
      width: columns * widest + (columns - 1) * Noo.fontSize()
      columns: Math.floor(parent.width / (widest + Noo.fontSize()))
      horizontalItemAlignment: Grid.AlignHCenter
      anchors.horizontalCenter: parent.horizontalCenter
      property real widest: 0
      spacing: Noo.fontSize()
      Repeater {
        model: [ "Tony Nederpel", "Jose1711", "Wayne Bonner", "Aaron Wolf",
                 "Torsten Philipp", "Vincent Bermel", "Tomasz Matuszewski",
                 "Yves Balhant", "Илья Б.", "...and others"
               ]
        Tile {
          id: donTile
          anchors.horizontalCenter: undefined
          property color randCol: Noo.randomColor()
          width: tt.width + Noo.fontSize() * 4
          bgBorder { color: randCol; width: 2 }
          bgColor: Qt.tint(randCol, Noo.alpha(activPal.base, 180))
          Text {
            id: tt
            font.pixelSize: Noo.fontSize() * 1.5
            text: modelData
            anchors.horizontalCenter: parent.horizontalCenter
            color: activPal.text
            Component.onCompleted: container.widest = Math.max(container.widest, tt.width + Noo.fontSize() * 4)
          }
          MouseArea {
            id: ma
            anchors.fill: parent
            hoverEnabled: true
            onEntered: {
              randCol = Noo.randomColor()
              donTile.scale = 1.5
            }
            onExited: donTile.scale = 1
            onClicked: randCol = Noo.randomColor()
          }
          Behavior on randCol { ColorAnimation { duration: 300 }}
          Behavior on scale { NumberAnimation { duration: 300}}
        }
      }
    }

    Text {
      font { bold: true; pixelSize: Noo.fontSize() * 2 }
      text: "THANK YOU!   "
      anchors.horizontalCenter: parent.horizontalCenter
      color: activPal.text
    }
  }
}

