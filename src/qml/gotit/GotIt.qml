/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
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
  property alias againChecked: showAgainChB.checked
  property alias againVisible: showAgainChB.visible

  width: parent.width; height: parent.height
  scale: GLOB.useAnimations ? 0 : 1.0
  enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1.0 }}
  exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0 }}
  padding: 0

  background: Rectangle { color: Noo.alpha(activPal.base, 235) }

  TgotIt { id: gotIt }

  RectangularGlow {
    z: 10
    anchors.fill: bg
    glowRadius: Noo.factor() / 2
    color: activPal.text
    cornerRadius: bg.radius + glowRadius
  }

  Rectangle {
    id: bg
    z: 10
    width: gotColl.width + 2 * radius
    height: gotColl.height + 1.5 * radius
    radius: (gotColl.height + 4 * Noo.factor()) / 4
    x: parent.width - width + radius; y: parent.height - height + radius + Noo.factor()
    color: ma.containsMouse ? activPal.highlight : Qt.tint(activPal.base, Noo.alpha(activPal.highlight, 150))
    transformOrigin: Item.TopRight
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
    Item { width: Noo.factor(); height: Noo.factor() / (showAgainChB.visible ? 2 : 1) }
    Text {
      id: gotText
      anchors { right: parent.right; rightMargin: Noo.factor() }
      color: activPal.text
      font { pixelSize: Noo.factor() * 3; bold: true }
      style: ma.containsMouse ? Text.Sunken : Text.Normal
      styleColor: activPal.base
      text: qsTr("GOT IT!")
    }
    TcheckBox {
      id: showAgainChB
      text: qsTranslate("ThelpDialogBase", "always show this help window")
    }
    Item { width: Noo.factor(); height: Noo.factor() / 2 }
  }
}


