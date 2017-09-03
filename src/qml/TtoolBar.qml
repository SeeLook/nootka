/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelapook@gmail.com)          *
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
    id: metroButt
    property TempoMenu menu: null
    width: settAct.width * 1.2; height: settAct.height
    x: pitchView.x - width
    onClicked: {
      if (!menu) {
        var c = Qt.createComponent("qrc:/TempoMenu.qml")
        menu = c.createObject(metroButt)
      }
      menu.open()
      metroButt.focus = false
    }
    background: Rectangle {
      anchors.fill: parent; color: activPal.window
      border { width: 2; color: SOUND.listening ? activPal.text : disdPal.text }
    }
    Rectangle {
      id: mic
      visible: SOUND.listening
      color: activPal.text
      y: parent.height / 15; width: parent.width / 5; height: parent.height / 8
      property int phase: 0
      Timer {
        running: SOUND.listening && (!metroButt.menu || metroButt.menu.tickEnable); repeat: true
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
          if (phase > 4) phase = 0
          mic.x = mic.width * phase
        }
      }
    }
    Text {
      y: parent.height / 3
      anchors.horizontalCenter: parent.horizontalCenter
      font { family: "Scorek"; pixelSize: parent.height / 4 }
      text: "\ue1d5=" + SOUND.tempo
      color: SOUND.listening ? activPal.text : disdPal.text
    }
  }
  PitchView {
    id: pitchView
    active: SOUND.listening
    anchors.right: label.left
    height: parent.height * 0.9
    width: parent.width * 0.4
    onPaused: {
      SOUND.stoppedByUser = !SOUND.stoppedByUser
      if (SOUND.listening)
        SOUND.stopListen()
      else
        SOUND.startListen()
    }
    Timer {
      repeat: true; interval: 75; running: SOUND.listening
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
