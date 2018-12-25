/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import "../"


Tflickable {
  height: parent.height

  property alias color: nooLab.bgColor

  contentHeight: aboutCol.height
  contentWidth: width

  Column {
    id: aboutCol
    width: parent.width
    spacing: Noo.fontSize()
    TipRect {
      height: Noo.fontSize() * 7; width: parent.width
      color: nooLab.bgColor; radius: 0
      NootkaLabel {
        id: nooLab
        height: parent.height
        active: false
        anchors.centerIn: parent
        bgColor: Noo.randomColor()
      }
      MouseArea {
        anchors.fill: parent
        onClicked: nooLab.bgColor = Noo.randomColor()
      }
    }
    Tile {
      bgColor: Qt.tint(nooLab.bgColor, Noo.alpha(activPal.base, 230))
      bgBorder { width: 2; color: nooLab.bgColor }
      width: parent.width - Noo.fontSize()
      Column {
        width: parent.width
        spacing: Noo.fontSize()
        Text {
          width: parent.width
          font { pixelSize: Noo.fontSize() * 2; bold: true }
          horizontalAlignment: Text.AlignHCenter
          text: "Nootka " + Noo.version()
          color: activPal.text
        }
        Text {
          text: "Thanks You are here!<br>This is beta version with functionality almost like older Nootka,<br>but with a brand new score with rhythm support and possibility to load/save music XML files.<br>Also there is a new look and layout and there are new instruments (piano, saxophone and bandoneon) along with guitar.<br>Anyway, main purpose of this release is to check all of that and give some feedback.<br>So happy testing,<br>Author<br><br>P.S.<br>Let me know what do you think..."
          width: parent.width * 0.96
          anchors.horizontalCenter: parent.horizontalCenter
          font { pixelSize: Noo.fontSize() * 1.1 }
          horizontalAlignment: Text.AlignLeft
          wrapMode: Text.WordWrap
          color: activPal.text
        }
      }
    }
  }
}
