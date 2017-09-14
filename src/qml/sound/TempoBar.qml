/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Item {
  id: root

  // private
  property var menu: null
//   property var rot: [ -18, -18, 0, 18, 18 ]
  property int hiTick: -1

  Rectangle {
    id: metroText
    width: height * 2.5; height: parent.height
    color: ma.containsMouse ? activPal.highlight : "transparent"
    radius: height / 5
    Text {
      anchors.horizontalCenter: parent.horizontalCenter
      y: parent.height * -0.7
      x: width * 0.2
      font { family: "Scorek"; pixelSize: parent.height * 0.7 }
      text: "\ue1d5=" + SOUND.tempo
      color: /*SOUND.listening ?*/ activPal.text //: disdPal.text
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
      x: root.width / 4 + index * (parent.height)
      y: (parent.height * 0.1) * (2 - Math.abs(2 - index))
      width: parent.height * 0.5; height: parent.height * 0.6 + Math.abs(2 - index) * 0.3 * parent.height
      radius: width / 2
      color: index === hiTick && timer.running ? activPal.text : activPal.button
      rotation: (index - 2) * 25 // rot[index]
    }
  }

  Timer {
    id: timer
    running: SOUND.listening && (!menu || menu.tickEnable); repeat: true
    interval: 60000 / SOUND.tempo / 4
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
      interval = Math.max(60000 / SOUND.tempo / 4 - lag, 1)
      lag = 0
      phase++
      if (phase > 7) phase = 0
      hiTick = Math.abs(phase - 4)
    }
  }
}
