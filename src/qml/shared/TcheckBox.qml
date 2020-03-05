/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


CheckBox {
  id: chB
  property alias textColor: content.color

  font.pixelSize: Noo.fontSize()

  indicator: TipRect {
    implicitWidth: Noo.fontSize() * 1.75; implicitHeight: Noo.fontSize() * 1.75
    x: chB.leftPadding; y: (chB.height - height) / 2
    horizontalOffset: rised ? Noo.fontSize() / 8 : 0; verticalOffset: horizontalOffset
    rised: !chB.down
    color: chB.enabled ? activPal.base : disdPal.base
    scale: chB.pressed ? 0.9 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}

    Rectangle {
      width: parent.width; height: parent.height / 7
      anchors.centerIn: parent
      color: chB.enabled ? activPal.dimText : disdPal.text
      rotation: 45; radius: height / 2
      visible: chB.checked
    }
    Rectangle {
      width: parent.width; height: parent.height / 7
      anchors.centerIn: parent
      color: chB.enabled ? activPal.dimText : disdPal.text
      rotation: 135; radius: height / 2
      visible: chB.checked
    }
  }

  contentItem: Text {
    id: content
    y: (chB.height - height) / 2
    text: chB.text
    topPadding: chB.font.pixelSize * 0.4
    leftPadding: indicator.width + chB.font.pixelSize / 2
    color: chB.enabled ? activPal.text : disdPal.text
    font { pixelSize: chB.font.pixelSize; family: chB.font.family }
  }
}
