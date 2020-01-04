/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Rectangle {
  id: cb

  property real factor: 1
  property bool selected: false
  property alias text: txt.text
  property alias font: txt.font
  property alias textHeight: txt.height
  property alias textColor: txt.color
  property alias yOffset: txt.y

  signal entered()
  signal exited()
  signal clicked()

  color: selected ? activPal.highlight : "transparent"
  width: factor * 2
  height: factor * 3
  radius: factor / 5

  scale: ma.containsMouse ? (ma.pressed ? 0.8 : 1.5) : 1
  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  Behavior on color { enabled: GLOB.useAnimations; ColorAnimation { duration: 150 }}

  Text {
    id: txt
    x: (cb.width - width) / 2
    style: Text.Normal
    color: cb.enabled ? activPal.text : disdPal.text
  }

  MouseArea {
    id: ma
    anchors.fill: parent
    hoverEnabled: true
    onClicked: cb.clicked()
    onEntered: cb.entered()
    onExited: cb.exited()
  }
}
