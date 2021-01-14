/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

import Nootka 1.0
import "sound"


TmobileMenu {
  id: root

  property Item toolBar: null // fake, for main window information
  property alias drawer: mainDrawer

  // private
  property var scoreMenu: null
  property var tempoMenu: null

  function open() {
    if (!scoreMenu) {
      var c = Qt.createComponent("qrc:/ScoreMenuContent.qml")
      scoreMenu = c.createObject(nootkaWindow.contentItem.parent, { "width": Noo.fontSize() * 15, "z": -1 })
    }
    scoreMenu.open()
  }

  onClicked: mainDrawer.open()

  parent: nootkaWindow.contentItem
  z: 550
  width: fingerPixels()
  height: fingerPixels() * 1.3

  Rectangle {
    id: bg
    width: fingerPixels() * 0.7; height: fingerPixels() * 1.3; x: fingerPixels() / 10; y:  fingerPixels() / 10;
    color: Noo.alpha(activPal.highlight, pressed ? 255 : 25)
    radius: fingerPixels() / 10
    Column {
      width: parent.width
      spacing: bg.width / 4
      topPadding: spacing
      Rectangle {
        width: bg.width / 4; height: width; radius: width / 2
        anchors.horizontalCenter: parent.horizontalCenter
        color: SOUND.playing ? "lime" : "black"
      }
      Rectangle {
        width: bg.width / 4; height: width; radius: width / 2
        anchors.horizontalCenter: parent.horizontalCenter
        color: score.recordMode ? "red" : "black"
      }
      Rectangle {
        id: pitchDot
        width: bg.width / 4; height: width; radius: width / 2
        anchors.horizontalCenter: parent.horizontalCenter
        color: SOUND.listening ? "mediumturquoise" : "black"
      }
    }
  }

  Taction {
    id: tunerAct
    text: Noo.TR("TunerDialog", "Nooter - Nootka tuner").replace("-", "<br><font size=\"1\">") + "</font>"
    icon: "fork"
    onTriggered: {
      nootkaWindow.showDialog(Nootka.Tuner)
      SOUND.startListen()
    }
  }

  Connections {
    target: tempoAct
    onTriggered: {
      if (!tempoMenu)
        tempoMenu = Qt.createComponent("qrc:/sound/TempoMenu.qml").createObject(nootkaWindow.contentItem)
      tempoMenu.open()
    }
  }

  Drawer {
    id: mainDrawer
    property NootkaLabel label: null
    width: Noo.fontSize() * 20; height: nootkaWindow.height
    onVisibleChanged: {
      if (visible) {
          if (!drawerLoad.active)
            drawerLoad.active = true
          label.bgColor = Noo.randomColor()
          SOUND.stopListen()
      } else {
          if (!dialogLoader || !dialogLoader.visible)
            SOUND.startListen()
      }
    }

    Loader {
      id: drawerLoad
      active: false
      anchors.fill: parent
      sourceComponent: Component {
        Flickable {
          anchors.fill: parent
          clip: true
          contentHeight: drawerColumn.height
          Column {
            id: drawerColumn
            width: parent.width
            spacing: fingerPixels() / 8
            NootkaLabel {
              id: nooLabel
              height: Noo.fontSize() * 7.91015625 // (logo ratio) 0.3955078125 * 20
              enabled: !GLOB.isExam
              onClicked: {
                mainDrawer.close()
                Noo.aboutAct.trigger()
              }
              Component.onCompleted: mainDrawer.label = this
            }
            Repeater {
              model: examActions
              MenuButton {
                action: modelData
                visible: action && action.enabled
                Rectangle {
                  width: Noo.fontSize() * 3; height: parent.height
                  z: -1
                  color: action ? Noo.alpha(action.bgColor, 50) : "transparent"
                }
              }
            }
            MenuButton { action: pitchDetectAct }
            MenuButton {
              visible: !GLOB.singleNoteMode
              action: tempoAct
              Text {
                property var beatModel: [ "\ue1d5", "\ue1d7", "\ue1d5 \ue1e7", "\ue1d3" ]
                x: parent.width - width - Noo.fontSize(); y: parent.height * -0.55
                font { family: "Scorek"; pixelSize: parent.height * 0.6 }
                text: beatModel[SOUND.beatUnit] + "=" + SOUND.tempo
              }
            }
            MenuButton { action: Noo.examAct }
            MenuButton { action: Noo.levelAct }
            MenuButton { visible: !GLOB.singleNoteMode; action: Noo.scoreAct }
            MenuButton {
              action: Taction {
                text: Noo.settingsAct.text
                icon: GLOB.isExam ? "exam-settings" : "systemsettings"
                onTriggered: Noo.settingsAct.trigger()
              }
            }
            MenuButton { action: tunerAct }
            MenuButton { onClicked: nootkaWindow.close(); action: Taction { icon: "close"; text: Noo.TR("QShortcut", "Close") } }
          }
        }
      }
    }
  }

  onFlyClicked: {
    if (currentFly)
      currentFly.taction.trigger()
  }

  Connections {
    target: SOUND
    onVolumeKeyPressed: nootkaWindow.showDialog(Nootka.Tuner)
    onListeningChanged: pitchDot.scale = 1
  }

  FlyItem { taction: fly1act; index: 0 }
  FlyItem { taction: fly2act; index: 1 }
  FlyItem { taction: fly3act; index: 2 }
  FlyItem { taction: fly4act; index: 3 }
  FlyItem { taction: fly5act; index: 4 }

  Timer {
    repeat: true; interval: 75
    running: SOUND.listening
    onTriggered: pitchDot.scale = 1 + SOUND.inputVol()
  }

}
