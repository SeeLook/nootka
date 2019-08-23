/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
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
        hoverEnabled: true
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
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
        LinkText {
          text: "Thanks You decided to test this beta version!<br>
          It works \"almost\" like previous Nootka, but with a brand new score with rhythms support and possibility to load/save music XML files.<br>
          Also there is a new look and a layout and there are new instruments (piano, saxophone and bandoneon) along with guitars.<br>
          <br>
          For more details about news in this release take a look at 'Changes' page.<br>
          Also <a href=\"https://sourceforge.net/p/nootka/hg/ci/default/tree/TODO.md\">here is a link to TODO list</a> to explain what is planed next.<br>
          <br>
          Anyway, main purpose of this release is to check all of that and give some feedback.<br>
          So happy testing,<br>
          Author<br>
          <br>
          P.S.<br>
          <a href=\"mailto:seelook.gmail.com\">Let me know what do you think...</a>"
          width: parent.width * 0.96
          anchors.horizontalCenter: parent.horizontalCenter
          font { pixelSize: Noo.fontSize() * 1.1 }
          horizontalAlignment: Text.AlignLeft
          wrapMode: Text.WordWrap
        }
      }
    }
  }
}
