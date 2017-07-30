/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


ToolBar {
  id: toolBar

  property alias scoreAct: scoreAct

  height: settAct.height
  background: Rectangle { anchors.fill: parent; color: activPal.window }

  Row {
    HeadButton { id: settAct; action: nootkaWindow.settingsAct }
    HeadButton { action: nootkaWindow.levelAct }
    HeadButton { id: scoreAct; action: nootkaWindow.scoreAct }
    HeadButton { action: nootkaWindow.examAct }
  }

  ToolButton {
    id: metro
    property TempoMenu menu: null
    width: settAct.width * 1.2; height: settAct.height
    x: pitchView.x - width
    Rectangle {
      id: pend
      visible: !metro.menu || metro.menu.tickEnable
      y: parent.height / 15; width: parent.width / 15; height: parent.height / 4
      color: activPal.text
      SequentialAnimation on x {
        id: metroAnim
        loops: Animation.Infinite; running: true
        NumberAnimation { duration: 60000 / SOUND.tempo; from: 0; to: metro.width - pend.width }
        NumberAnimation { duration: 60000 / SOUND.tempo; from: metro.width - pend.width; to: 0 }
      }
    }
    onClicked: {
      if (!menu) {
        var c = Qt.createComponent("qrc:/TempoMenu.qml")
        menu = c.createObject(metro)
        menu.onAccepted.connect(metroAnim.restart)
      }
      menu.open()
    }
    background: Rectangle {
      anchors.fill: parent; color: activPal.window
      border { width: 2; color: activPal.text }
    }
//     Text {
//       id: mic
//       anchors.horizontalCenter: parent.horizontalCenter
//       font { family: "Nootka"; pixelSize: parent.height / 2 }
//       text: "r"
//       Timer {
//         running: true; repeat: true
//         interval: 150
//         onTriggered: {
//           if (interval === 150) {
//             mic.color = activPal.text
//               interval = 60000 / SOUND.tempo - 150
//           } else {
//               mic.color = "red"
//               interval = 150
//           }
//         }
//       }
//     }
    Text {
      y: parent.height / 3
      anchors.horizontalCenter: parent.horizontalCenter
      font { family: "Scorek"; pixelSize: parent.height / 4 }
      text: "\ue1d5=" + SOUND.tempo
    }
  }
  PitchView {
    id: pitchView
    x: label.x - parent.width * 0.41; y: parent.height * 0.05
    height: parent.height * 0.9
    width: parent.width * 0.4
    Timer {
      repeat: true; interval: 75; running: true
      onTriggered: {
        pitchView.volume = SOUND.inputVol()
        pitchView.deviation = SOUND.pitchDeviation()
      }
    }
  }

  NootkaLabel {
    id: label
    anchors.right: parent.right
    height: toolBar.height
    onClicked: nootkaWindow.aboutAct.trigger()
  }

}
