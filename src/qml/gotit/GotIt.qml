/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
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

  scale: GLOB.useAnimations ? 0 : 1.0
  enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1.0 }}
  exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0 }}

  width: parent.width; height: parent.height
  padding: 0

  background: Rectangle { color: activPal.base }

  TgotIt { id: gotIt }

  RectangularGlow {
    z: 10
    anchors.fill: bg
    glowRadius: Noo.factor() / 2
    color: activPal.text
    cornerRadius: bg.radius + glowRadius
    transformOrigin: Item.BottomRight
    scale: GLOB.useAnimations && ma.pressed ? 0.95 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  }

  Rectangle {
    id: bg
    z: 10
    width: gotColl.width + 2 * radius
    height: gotColl.height + 1.5 * radius
    radius: (gotColl.height + 4 * Noo.factor()) / 4
    x: parent.width - width + radius + Noo.factor(); y: parent.height - height + radius + Noo.factor()
    color: ma.containsMouse ? activPal.highlight : Qt.tint(activPal.base, Noo.alpha(activPal.highlight, 150))
    transformOrigin: Item.BottomRight
    scale: GLOB.useAnimations && ma.pressed ? 0.95 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    MouseArea {
      id: ma
      anchors.fill: parent
      hoverEnabled: !Noo.isAndroid()
      onClicked: pop.clicked()
    }
  }

  Column {
    id: gotColl
    z: 10
    x: parent.width - width; y: parent.height - height
    transformOrigin: Item.BottomRight
    scale: GLOB.useAnimations && ma.pressed ? 0.95 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    Text {
      id: gotText
      anchors.horizontalCenter: parent.horizontalCenter
      color: activPal.text
      font { pixelSize: Noo.factor() * 2; bold: true }
      style: ma.containsMouse ? Text.Sunken : Text.Normal
      styleColor: activPal.base
      text: qsTr("GOT IT!")
    }
    TcheckBox {
      id: remaindChB
      text: qsTr("remind me next time")
    }
    Item { width: Noo.factor(); height: Noo.factor() / 2 }
  }
}


