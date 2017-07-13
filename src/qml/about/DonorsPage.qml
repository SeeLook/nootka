/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Flickable {
  id: flick
  clip: true
  ScrollBar.vertical: ScrollBar {}
  contentHeight: donCol.height; contentWidth: parent.width

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
      model: [ "Илья Б.", "Yves Balhant", "Tomasz Matuszewski", "Vincent Bermel" ]
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
  }
}

