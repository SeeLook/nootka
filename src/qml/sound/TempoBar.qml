/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Item {
  id: root

  // private
  property var menu: null
  property int hiTick: -1
  property int cnt: 1
  property var hArray: [ 0.6, 0, 0.3, 0, 0.6]
  property var gArray: [ "\ue1d5", "\ue1d9", "\ue1d7", "\ue1d9", "\ue1d5" ]

  Rectangle {
    id: metroText
    width: height * 2.5; height: parent.height
    color: ma.containsMouse ? activPal.highlight : (Noo.isAndroid() ? activPal.base : "transparent")
    radius: height / 5
    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      y: parent.height * -0.7
      x: width * 0.2
      font { family: "Scorek"; pixelSize: parent.height * 0.7 }
      text: "\ue1d5=" + SOUND.tempo
      color: activPal.text
    }
    MouseArea {
      id: ma
      hoverEnabled: true
      anchors.fill: parent
      onClicked: {
        if (!menu) {
          var c = Qt.createComponent("qrc:/sound/TempoMenu.qml")
          menu = c.createObject(root)
        }
        menu.open()
      }
    }
  }

  Repeater {
    id: rep
    model: 5
    Rectangle {
      readonly property color bgColor: Qt.tint(activPal.window, Noo.alpha(activPal.base, 100))
      x: root.width / 3 + index * (parent.height) - width / 2
      y:  parent.height * (Math.abs(0.6 - hArray[index]) / 5)
      width: parent.height * (0.9 - (Math.abs(0.6 - hArray[index]) / 3)); height: parent.height * 0.8 + hArray[index] * parent.height
      radius: width / 2
      color: index === hiTick && timer.running ? (index === 0 || index === 4 ? activPal.highlight : activPal.text) : bgColor
      rotation: (index - 2) * 30
      visible: SOUND.tempo < 110 || index % 2 !== 1
      Text { // rhythm
        visible: index !== hiTick || !SOUND.listening
        font { pixelSize: parent.height * 0.8; family: "Scorek" }
        color: disdPal.text
        text: gArray[index]
        y: parent.height * -0.85
        x: (parent.width - width) / 2
      }
      Text { // count
        visible: SOUND.listening && index === hiTick && (!menu || (menu.count && menu.tickEnable))
        font { pixelSize: parent.height; family: "Scorek" }
        color: activPal.base
        text: cnt
        y: parent.height * -1.2
        x: (parent.width - width) / 2
      }
    }
  }

  Timer {
    id: timer
    running: visible && SOUND.listening && (!menu || menu.tickEnable); repeat: true
    interval: (SOUND.tempo < 110 ? 15000 : 30000) / SOUND.tempo
    property real elap: 0
    property real lag: 0
    property int phase: 0
    onTriggered: {
      var currTime = new Date().getTime()
      if (elap > 0) {
        elap = currTime - elap
        lag += elap - interval
      }
      elap = currTime
      interval = Math.max(((SOUND.tempo < 110 ? 15000 : 30000) / SOUND.tempo) - lag, 1)
      lag = 0
      if ((phase + (SOUND.tempo < 110 ? 1 : 2)) % 4 === 0) {
        if (cnt < 4)
          cnt++
        else
          cnt = 1
      }
      phase += SOUND.tempo < 110 ? 1 : 2
      if (phase > 7) phase = 0
      hiTick = Math.abs(phase - 4)
    }
  }
}
