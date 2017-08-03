/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0


Rectangle {
  width: parent.height * 3.6; height: parent.height
  color: "black"
  property alias note: bg.note

  property real factor: height / 112 //182.89940

  TbandoneonBg { id: bg }

  Repeater {
    model: 33

    RoundButton {
      width: parent.height / 8.5; height: width
      x: bg.leftXat(index) * factor //- width / 2
      y: bg.leftYat(index) * factor + width / 3
      autoExclusive: true
      checkable: true
      background: Rectangle {
        color: parent.checked ? (bellowsButt.checked ? "pink" : "gray") : "white"
        radius: width / 2
      }
      onClicked: bg.currentIndex = index
    }
  }
  
  Button {
    id: bellowsButt
    anchors { horizontalCenter: parent.horizontalCenter; bottom: parent. bottom }
    text: checked ? qsTr("bellows<br>opening") : qsTr("bellows<br>closing")
    checkable: true
    background: Rectangle {
      color: bellowsButt.checked ? "pink" : "gray"
    }
    onClicked: bg.closing = !checked
  }

  Repeater {
    model: 38
    RoundButton {
      width: parent.height / 8.5; height: width
      x: (bg.rightXat(index)) * factor  + parent.width * 0.55
      y: (bg.rightYat(index)) * factor + width
      autoExclusive: true
      checkable: true
      background: Rectangle {
        color: parent.checked ? (bellowsButt.checked ? "pink" : "gray") : "white"
        radius: width / 2
      }
      onClicked: bg.currentIndex = index + 33
    }
  }

}

