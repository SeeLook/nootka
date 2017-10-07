/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Flickable {
  property int instrument: 1

  clip: true
  contentHeight: instrCol.height
  contentWidth: width

  Column {
    id: instrCol
    width: parent.width
    spacing: Noo.fontSize() * 3
    Text {
      text: qsTr("What instrument do you play?")
      font { pixelSize: Noo.fontSize() * 2; bold: true }
      width: parent.width
      wrapMode: Text.WordWrap
      horizontalAlignment: Text.AlignHCenter
      color: activPal.text
    }
    Column {
      anchors.horizontalCenter: parent.horizontalCenter

      Repeater {
        model: 8
        Row {
          width: Noo.fontSize() * 30
          spacing: Noo.fontSize() * 3
          Text {
            font {family: "nootka"; pixelSize: Noo.fontSize() * 5 }
            text: Noo.instr(index).glyph
            color: instrument === index ? activPal.highlight : activPal.text
            scale: instrument === index ? 1.4 : 1.0
            x: instrument === index ? -Noo.fontSize() / 3 : 0
            Behavior on x { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.1; duration: 500 }}
            Behavior on scale { enabled: GLOB.useAnimations; PropertyAnimation { duration: 300 }}
            MouseArea {
              anchors.fill: parent
              onClicked: instrument = index
            }
          }
          Text {
            id: instrText
            text: Noo.instr(index).name
            font { pixelSize: Noo.fontSize() * (instrument === index ? 1.4 : 1.0); bold: true }
            color: index === instrument ? activPal.highlight : activPal.text
            anchors.verticalCenter: parent.verticalCenter
            MouseArea {
              anchors.fill: parent
              onClicked: instrument = index
            }
          }
        }
      }
    }
  }

  ScrollBar.vertical: ScrollBar { active: !Noo.isAndroid() }
}
