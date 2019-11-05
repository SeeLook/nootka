/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


RadioButton {
  id: rb
  property alias textColor: content.color
  property real textScale: 1.0

  font.pixelSize: Noo.fontSize()

  indicator: TipRect {
    implicitWidth: rb.font.pixelSize * 2; implicitHeight: rb.font.pixelSize * 2
    x: rb.leftPadding; y: (rb.height - height) / 2
    horizontalOffset: rised ? Noo.fontSize() / 8 : 0; verticalOffset: horizontalOffset
    radius: implicitWidth / 2
    rised: !rb.down
    color: rb.enabled ? activPal.base : disdPal.base
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
  }
}
