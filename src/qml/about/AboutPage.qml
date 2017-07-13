/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import "../"


Flickable {
  width: parent.width
  height: parent.height

  contentHeight: aboutCol.height
  contentWidth: width

  Column {
    id: aboutCol
    width: parent.width
    spacing: nootkaWindow.fontSize
    Rectangle {
      height: nootkaWindow.fontSize * 7
      width: parent.width
      color: nooLab.bgColor
      NootkaLabel {
        id: nooLab
        height: parent.height
        active: false
        anchors.centerIn: parent
        bgColor: Noo.randomColor()
      }
    }
    Tile {
      bgColor: Qt.tint(nooLab.bgColor, Qt.rgba(activPal.base.r, activPal.base.g, activPal.base.b, 0.9))
      bgBorder { width: 2; color: nooLab.bgColor }
      width: parent.width * 0.9
      Column {
        width: parent.width
        spacing: nootkaWindow.fontSize
        Text {
          width: parent.width
          font { pixelSize: nootkaWindow.fontSize * 2; bold: true }
          horizontalAlignment: Text.AlignHCenter
          text: "Nootka " + Noo.version()
          color: activPal.text
        }
        Text {
          text: "A few nice words to introduce Nootka shortly."
          width: parent.width
          font { pixelSize: nootkaWindow.fontSize * 1.2 }
          horizontalAlignment: Text.AlignHCenter
          color: activPal.text
        }
      }
    }
  }
}
