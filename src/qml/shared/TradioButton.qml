/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Nootka 1.0

RadioButton {
  id: rb
  property alias textColor: content.color
  property real textScale: 1.0

  font.pixelSize: NOO.factor()

  indicator: TipRect {
    implicitWidth: rb.font.pixelSize * 2; implicitHeight: rb.font.pixelSize * 2
    x: rb.leftPadding; y: (rb.height - height) / 2
    horizontalOffset: rised ? NOO.factor() / 8 : 0; verticalOffset: horizontalOffset
    radius: implicitWidth / 2
    rised: !rb.down
    color: rb.enabled ? activPal.base : Qt.darker(disdPal.window, 1.2)
    scale: rb.pressed ? 0.8 : (rb.checked ? 0.9 : 1.0)
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}

    Rectangle {
      anchors.fill: parent
      radius: width / 2; scale: 0.6
      color: rb.enabled ? activPal.dimText : disdPal.text
      rotation: 45
      visible: rb.checked
    }
  }

  contentItem: Text {
    id: content
    text: rb.text
    topPadding: rb.font.pixelSize * 0.4
    leftPadding: indicator.width + rb.font.pixelSize / 2
    color: rb.enabled ? activPal.text : disdPal.text
    font { pixelSize: rb.font.pixelSize * textScale; family: rb.font.family }
    style: rb.activeFocus ? Text.Sunken : Text.Normal
    styleColor: activPal.highlight
  }
}
