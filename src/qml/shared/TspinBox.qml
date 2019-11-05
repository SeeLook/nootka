/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


SpinBox {
  id: sb

  editable: true
  height: Noo.fontSize() * 3; width: Noo.fontSize() * 12
  font.pixelSize: Noo.fontSize()

  contentItem: TextInput {
    z: 2
    width: Noo.fontSize() * 6
    text: sb.textFromValue(sb.value, sb.locale)
    
    font: sb.font
    color: sb.enabled ? activPal.text : disdPal.text
    selectionColor: activPal.highlight
    selectedTextColor: activPal.highlightedText
    horizontalAlignment: Qt.AlignHCenter; verticalAlignment: Qt.AlignVCenter
    
    readOnly: !sb.editable
    validator: sb.validator
    inputMethodHints: Qt.ImhFormattedNumbersOnly
  }

  background: Rectangle {
    color: sb.enabled ? activPal.base : disdPal.base
    width: Noo.fontSize() * 6; height: Noo.fontSize() * 3
    x: Noo.fontSize() * 3
  }

  up.indicator: TipRect {
    x: sb.mirrored ? 0 : sb.width - sb.height - Noo.fontSize() / 5
    implicitHeight: Noo.fontSize() * 3; implicitWidth: Noo.fontSize() * 2.8
    color: sb.enabled ? activPal.button : disdPal.button
    rised: !sb.up.pressed
    scale: sb.up.pressed ? 0.9 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    Rectangle {
      x: parent.width / 4; width: parent.width / 2; height: parent.height / 15; y: parent.height * 0.48
      color: sb.enabled ? activPal.dimText : disdPal.text
    }
    Rectangle {
      x: parent.width / 4; width: parent.width / 2; height: parent.height / 15; y: parent.height * 0.48
      color: sb.enabled ? activPal.dimText : disdPal.text
      rotation: 90
    }
  }
  
  down.indicator: TipRect {
    x: sb.mirrored ? sb.width - sb.height - Noo.fontSize() / 5 : 0
    implicitHeight: Noo.fontSize() * 3; implicitWidth: Noo.fontSize() * 2.8
    color: sb.enabled ? activPal.button : disdPal.button
    rised: !sb.down.pressed
    scale: sb.down.pressed ? 0.9 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    Rectangle {
      x: parent.width / 4; width: parent.width / 2; height: parent.height / 15; y: parent.height * 0.48
      color: sb.enabled ? activPal.dimText : disdPal.text
    }
  }
}
