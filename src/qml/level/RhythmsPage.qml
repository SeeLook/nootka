/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tflickable {
  id: rtmPage
  contentHeight: rtmCol.height + Noo.fontSize() * 2

  Column {
    id: rtmCol
    width: parent.width; topPadding: Noo.fontSize()

    Tframe {
      width: parent.width * 0.99
      Column {
        width: parent.width; spacing: Noo.fontSize() / 2
        TcheckBox {
          id: rtmEnabledChB
          checked: true
          anchors.horizontalCenter: parent.horizontalCenter
          text: qsTr("Use rhythms in this level")
        }
        Text {
          text: qsTr("It must be checked, otherwise any melodies will not have rhythmic values, just bare note-heads.")
          anchors.horizontalCenter: parent.horizontalCenter
          width: parent.width * 0.96
          font { pixelSize: Noo.fontSize() * 0.8; bold: true }
          textFormat: Text.RichText; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap
        }
        Tile {
          enabled: rtmEnabledChB.checked
          MeterSelector {
            width: parent.width
          }
          description: "some tile"
        }
        Text {
          text: "\ue1f0 \ue1f2\ue1f7"
          anchors.horizontalCenter: parent.horizontalCenter
          font { pixelSize: Noo.fontSize() * 2; family: "Scorek" }
        }
      }
    }
  }
}

