/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tile {
  property int qId: 0
  property alias questionText: questionChB.text
  property alias questionChecked: questionChB.checked

  property var answSymb: [ "s", "c", "g", "n" ]

  width: col.width + Noo.fontSize() * 4

  Column {
    id: col
    anchors.horizontalCenter: parent.horizontalCenter
    Row {
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: Noo.fontSize() / 2
      TcheckBox { id: questionChB }
      Text {
        text: answSymb[qId] + "?"
        anchors.verticalCenter: parent.verticalCenter
        font { family: "nootka"; pixelSize: Noo.fontSize() * 2 }
        color: activPal.text
      }
    }
    Row {
      spacing: Noo.fontSize() / 2
      anchors.horizontalCenter: parent.horizontalCenter
      Text { text: qsTr("answers") + ":"; anchors.verticalCenter: parent.verticalCenter }
      Repeater {
        model: 4
        TcheckBox {
          text: answSymb[index]
          font { family: "nootka"; pixelSize: Noo.fontSize() * 2 }
        }
      }
    }
  }
}
