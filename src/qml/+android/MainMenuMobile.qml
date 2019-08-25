/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
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

  function open() { mainDrawer.scoreMenu.open() }

  z: 1000
  width: fingerPixels()
  height: fingerPixels()

  Rectangle {
    id: bg
    width: fingerPixels() / 2; height: fingerPixels(); x: fingerPixels() / 10; y:  fingerPixels() / 10;
    color: Noo.alpha(activPal.highlight, pressed ? 255 : 25)
    radius: fingerPixels() / 10
    Column {
      width: parent.width
      spacing: fingerPixels() * 0.15625
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
        width: bg.width / 4; height: width; radius: width / 2
        anchors.horizontalCenter: parent.horizontalCenter
        color: SOUND.listening ? "blue" : "black"
      }
    }
  }

  Taction {
    id: pitchDetectAct
    text: qsTr("Pitch recognition", "Android menu entry, could be 'Note recognition' or 'detection' as well")
    icon: "delete"
    onTriggered: {
      SOUND.stoppedByUser = !SOUND.stoppedByUser
      if (SOUND.listening)
        SOUND.stopListen()
      else
        SOUND.startListen()
    }
  }

  property var tempoBar: null
  Taction {
    id: tempoAct
    text: qsTr("Metronome")
    icon: "fork"
    onTriggered: {
      if (!tempoBar) {
        var c = Qt.createComponent("qrc:/sound/TempoBar.qml")
        tempoBar = c.createObject(nootkaWindow.contentItem,
                  { "z": 10, "height": nootkaWindow.height * 0.08, "y": nootkaWindow.height * 0.85, "width": nootkaWindow.width / 2, "x": nootkaWindow.width / 4 })
        console.log("tempoBar", tempoBar)
      }
    }
  }

  Drawer {
    id: mainDrawer
    property Item scoreMenu: null
    property NootkaLabel label: null
    width: Noo.fontSize() * 20
    height: nootkaWindow.height
    onVisibleChanged: {
      if (visible) {
        if (!drawerLoad.active)
          drawerLoad.active = true
        label.bgColor = Noo.randomColor()
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
              onClicked: {
                mainDrawer.close()
                Noo.aboutAct.trigger()
              }
              Component.onCompleted: mainDrawer.label = this
            }
            MenuButton { action: pitchDetectAct; onClicked: mainDrawer.close() }
            MenuButton { action: tempoAct; onClicked: mainDrawer.close() }
            MenuButton { action: Noo.levelAct; onClicked: mainDrawer.close() }
            MenuButton { action: Noo.examAct; onClicked: mainDrawer.close() }
            MenuButton { action: Noo.settingsAct; onClicked: mainDrawer.close() }
            MenuButton { action: Noo.scoreAct }
            Column { // drop-down menu with score actions
              id: scoreMenu
              function open() { visible ? state = "Invisible" : state = "Visible" }
              spacing: fingerPixels() / 8
              width: parent.width - Noo.fontSize() / 2
              x: -parent.width
              visible: false
              MenuButton { action: score.playAct; onClicked: mainDrawer.close() }
              MenuButton { action: score.showNamesAct; onClicked: mainDrawer.close() }
//               MenuButton { action: score.extraAccidsAct; onClicked: mainDrawer.close() } // Not implemented yet
              MenuButton { action: score.zoomInAct; onClicked: mainDrawer.close() }
              MenuButton { action: score.zoomOutAct; onClicked: mainDrawer.close() }
              MenuButton { action: score.openXmlAct; onClicked: mainDrawer.close() }
              MenuButton { action: score.saveXmlAct; onClicked: mainDrawer.close() }
              MenuButton { action: score.clearScoreAct; onClicked: mainDrawer.close() }
              states: [ State { name: "Visible"; when: scoreMenu.visible }, State { name: "Invisible"; when: !scoreMenu.visible } ]

              transitions: [
                Transition {
                  from: "Invisible"; to: "Visible"
                  SequentialAnimation {
                    PropertyAction { target: scoreMenu; property: "visible"; value: true }
                    NumberAnimation { target: scoreMenu; property: "x"; to: Noo.fontSize() / 2; duration: 300 }
                  }
                },
                Transition {
                  from: "Visible"; to: "Invisible"
                  SequentialAnimation {
                    NumberAnimation { target: scoreMenu; property: "x"; to: -parent.width; duration: 300 }
                    PropertyAction { target: scoreMenu; property: "visible"; value: false }
                  }
                }
              ]
              Component.onCompleted: mainDrawer.scoreMenu = this
            }
            MenuButton { onClicked: nootkaWindow.close(); action: Taction { icon: "exit"; text: Noo.TR("QShortcut", "Exit") } }
          }
        }
      }
    }
  }


  onClicked: mainDrawer.open()

  onFlyClicked: {
    if (currentFly)
      currentFly.taction.trigger()
  }

  Repeater {
    model: flyActions
    FlyItem {
      visible: extra
      x: flyX(index); y: flyY(index); width: fingerPixels() * 1.5; height: fingerPixels() * 1.5
      taction: modelData
      color: currentFly === this ? activPal.highlight : activPal.button
    }
  }
  Component.onCompleted: {
    addAction(score.playAct)
    addAction(score.recModeAct)
    addAction(pitchDetectAct)
    addAction(score.clearScoreAct)
  }
}
