/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0


Item {
  width: childrenRect.width; height: parent.height
  property alias note: bg.note

  property real factor: height / 100

  anchors.horizontalCenter: parent.horizontalCenter

  TbandoneonBg { id: bg }

  Row {
    height: parent.height
    Rectangle {
      height: parent.height
      width: factor * 200
      color: "black"
      Repeater {
        model: 33
        RoundButton {
          width: parent.height / 8; height: width
          x: bg.leftXat(index) * factor //- width / 2
          y: bg.leftYat(index) * factor + width / 3
          autoExclusive: true
          checkable: true
          background: Rectangle {
            color: parent.checked ? (bellowsButt.checked ? "blue" : "green") : "white"
            radius: width / 2
          }
          onClicked: bg.currentIndex = index
        }
      }
    }

    Button {
      id: bellowsButt
      anchors.verticalCenter: parent.verticalCenter
      width: nootkaWindow.fontSize * 10
      text: checked ? qsTr("bellows<br>opening") : qsTr("bellows<br>closing")
      checkable: true
      background: Rectangle { color: bellowsButt.checked ? "blue" : "green" }
      onClicked: bg.closing = !checked
    }

    Rectangle {
      height: parent.height
      width: factor * 190
      color: "black"
      Repeater {
        model: 38
        RoundButton {
          width: parent.height / 8; height: width
          x: (bg.rightXat(index)) * factor // + parent.width * 0.55
          y: (bg.rightYat(index)) * factor + width / 2
          autoExclusive: true
          checkable: true
          background: Rectangle {
            color: parent.checked ? (bellowsButt.checked ? "blue" : "green") : "white"
            radius: width / 2
          }
          onClicked: bg.currentIndex = index + 33
        }
      }
    }

  }

}

