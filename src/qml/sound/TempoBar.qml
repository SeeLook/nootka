/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka 1.0
import "../"


Item {
  id: tempoBar

  // private
  property var tMenu: null
  property int hiTick: -1
  property int cnt: 1
  property var hArray: [ 0.6, 0, 0.3, 0, 0.6]
  property var gArray: [ "\ue1d5", "\ue1d9", "\ue1d7", "\ue1d9", "\ue1d5" ]
  property int countTo: Noo.meter(score.meter).countTo()

  // protected
  property var beatModel: [ "\ue1d5", "\ue1d7", "\ue1d5 \ue1e7", "\ue1d3" ]

  MouseArea {
    anchors.fill: parent
    hoverEnabled: true
    onEntered: Noo.setStatusTip(qsTr("Metronome"), Item.TopLeft)
    onExited: Noo.setStatusTip("", Item.TopLeft)
  }

  RectButton {
    id: metroText
    forcedHeight: parent.height; yOffset: parent.height * -0.7
    statusTip: qsTr("Tempo")
    font { family: "Scorek"; pixelSize: parent.height * 0.7 }
    text:  beatModel[SOUND.beatUnit] + "=" + SOUND.tempo
    onClicked: {
      if (!tMenu) {
        var c = Qt.createComponent("qrc:/sound/TempoMenu.qml")
        tMenu = c.createObject(tempoBar)
      }
      tMenu.open()
    }
  }

  RectButton {
    x: metroText.width + Noo.fontSize()
    font { family: "Nootka"; pixelSize: parent.height }
    text: "\u018f"
    statusTip: qsTr("Audible metronome.<br>Use earphones! Otherwise ticking will disturb proper pitch detection!")
    checked: SOUND.tickDuringPlay
    onClicked: SOUND.tickDuringPlay = !SOUND.tickDuringPlay
  }

  Repeater {
    id: rep
    model: 5
    Rectangle {
      readonly property color bgColor: Qt.tint(activPal.window, Noo.alpha(activPal.base, 100))
      x: tempoBar.width / 3 + index * (parent.height) - width / 2
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
        visible: pitchView.active && index === hiTick && (!tMenu || (tMenu.count && tMenu.tickEnable))
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
    running: visible && pitchView.active && (!tMenu || tMenu.tickEnable); repeat: true
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
        if (cnt < countTo)
          cnt++
        else
          cnt = 1
      }
      phase += SOUND.tempo < 110 ? 1 : 2
      if (phase > 7) phase = 0
      hiTick = Math.abs(phase - 4)
    }
  }

  RectButton {
    x: parent.width - width * 1.1
    text: "440Hz"
    font { pixelSize: parent.height * 0.8; bold: true }
    statusTip: qsTr("Tuner")
    onClicked: {
      nootkaWindow.showDialog(Nootka.Tuner)
      SOUND.startListen()
    }
  }
}
