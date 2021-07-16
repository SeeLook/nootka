/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0

import Nootka.Main 1.0
import "../"


Popup {
  id: pop

  signal clicked()

  property alias gotItType: gotIt.gotItType
  property alias remaindChecked: remaindChB.checked
  property alias remindVisible: remaindChB.visible
  property alias gotIt: gotIt
  property bool showGotIt: true

      /**
       * Functions called by @p HelpPage to perform particular task on help pages, if any.
       */
  function stop() {}
  function start() {}

  scale: GLOB.useAnimations ? 0 : 1.0
  enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1.0 }}
  exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0 }}

  width: parent.width; height: parent.height
  padding: 0

  background: Rectangle { color: activPal.base }

  TgotIt { id: gotIt }

  RectangularGlow {
    z: 10
    visible: showGotIt
    anchors.fill: bg
    glowRadius: NOO.factor() / 2
    color: activPal.text
    cornerRadius: bg.radius + glowRadius
    transformOrigin: Item.BottomRight
    scale: GLOB.useAnimations && ma.pressed ? 0.95 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  }

  Rectangle {
    id: bg
    visible: showGotIt
    z: 10
    width: gotColl.width + 2 * radius + NOO.factor()
    height: gotColl.height + 1.5 * radius
    radius: (gotColl.height + 4 * NOO.factor()) / 4
    x: parent.width - width + radius + NOO.factor(); y: parent.height - height + radius + NOO.factor()
    color: ma.containsMouse ? activPal.highlight : Qt.tint(activPal.base, NOO.alpha(activPal.highlight, 150))
    transformOrigin: Item.BottomRight
    scale: GLOB.useAnimations && ma.pressed ? 0.95 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    MouseArea {
      id: ma
      anchors.fill: parent
      hoverEnabled: !NOO.isAndroid()
      onClicked: pop.clicked()
    }
  }

  Column {
    id: gotColl
    visible: showGotIt
    z: 10
    x: parent.width - width - NOO.factor() / 2; y: parent.height - height
    transformOrigin: Item.BottomRight
    scale: GLOB.useAnimations && ma.pressed ? 0.95 : 1.0
    Text {
      id: gotText
      anchors.horizontalCenter: parent.horizontalCenter
      color: activPal.text
      font { pixelSize: NOO.factor() * 2; bold: true }
      style: ma.containsMouse ? Text.Sunken : Text.Normal
      styleColor: activPal.base; transformOrigin: Item.Top
      text: qsTr("GOT IT!")
      Behavior on scale { NumberAnimation { duration: 150 }}
      Timer {
        running: showGotIt
        interval: 2000; repeat: true
        onTriggered: {
          if (interval > 300) {
              interval = 200
              gotText.scale = 1.3
          } else {
              interval = 2000
              gotText.scale = 1
          }
        }
      }
    }
    Row {
      TcheckBox { id: remaindChB }
      Text {
        anchors.verticalCenter: parent.verticalCenter
        color: activPal.text
        text: qsTr("remind me next time")
      }
    }
    Item { width: NOO.factor(); height: NOO.factor() / 2 }
  }
}


