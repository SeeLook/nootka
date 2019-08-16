/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


AbstractButton {
  id: root
  font { pixelSize: Noo.fontSize(); bold: activeFocus }
  focus: true

  property alias radius: bg.radius
  property alias color: bg.color
  property alias textColor: butText.color

  scale: GLOB.useAnimations && pressed ? 0.9 : 1.0
  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}

  contentItem: Text {
    id: butText
    padding: Noo.fontSize() / 3
    font: root.font
    horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
    minimumPixelSize: 8
    fontSizeMode: Text.HorizontalFit
    color: enabled ? (checked ? activPal.highlightedText : activPal.text) : disdPal.text
    text: root.text
  }

  background: TipRect {
    id: bg
    color: enabled ? (root.checked ? activPal.highlight : activPal.button) : disdPal.button
    rised: !root.checked && !root.pressed
  }
}
