/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


AbstractButton {
  id: cutButt
  font.pixelSize: NOO.factor()
  focus: true

  property alias radius: bg.radius
  property alias color: bg.color
  property alias textColor: butText.color
  property alias border: bg.border

  scale: GLOB.useAnimations && pressed ? 0.8 : (cutButt.checked ? 0.9 : 1.0)
  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}

  contentItem: Text {
    id: butText
    padding: NOO.factor() / 3
    font: cutButt.font
    horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
    minimumPixelSize: 8
    fontSizeMode: Text.HorizontalFit
    color: enabled ? (checked ? activPal.highlightedText : activPal.text) : disdPal.text
    text: cutButt.text
    style: cutButt.activeFocus ? Text.Sunken : Text.Normal
    styleColor: activPal.highlight
  }

  background: GlowRect {
    id: bg
    color: enabled ? (cutButt.checked ? activPal.highlight : activPal.button) : disdPal.button
    rised: !cutButt.checked && !cutButt.pressed
  }
}
