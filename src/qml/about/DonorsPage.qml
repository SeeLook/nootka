/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Flickable {
  id: root
  clip: true
  ScrollBar.vertical: ScrollBar {}
  contentHeight: donCol.height; contentWidth: parent.width
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
    width: parent.width
    spacing: nootkaWindow.fontSize

    Tile {
      width: parent.width
      anchors.horizontalCenter: undefined
      bgColor: activPal.highlight
      Text {
        text: qsTranslate("TaboutNootka", "People and companies who gave material support for the Nootka project")
        width: parent.width * 0.9
        wrapMode: Text.WordWrap
        font { pixelSize: nootkaWindow.fontSize * 2; bold: true }
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.highlightedText
        horizontalAlignment: Text.AlignHCenter
      }
    }

    Repeater {
      model: [ "Torsten Philipp", "Vincent Bermel", "Tomasz Matuszewski", "Yves Balhant", "Илья Б." ]
      Tile {
        property color randCol: Noo.randomColor()
        width: tt.width + nootkaWindow.fontSize * 4
        bgBorder { color: randCol; width: 2 }
        bgColor: Qt.tint(randCol, Noo.alpha(activPal.base, 180))
        Text {
          id: tt
          font.pixelSize: nootkaWindow.fontSize * 1.5
          text: modelData
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text
        }
      }
    }

    Text {
      font { bold: true; pixelSize: nootkaWindow.fontSize * 2 }
      text: "THANK YOU!   "
      anchors.right: parent.right
      color: activPal.text
    }
  }
}

