/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


SpinBox {
  id: sb

  editable: true
  height: sb.font.pixelSize * 3; width: sb.font.pixelSize * 12
  font.pixelSize: Noo.factor()

  contentItem: TextInput {
    z: 2
    width: sb.width - 2 * sb.height
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
    color: sb.enabled ? activPal.base : Qt.darker(disdPal.window, 1.2)
    width: sb.width - sb.height; height: sb.height
    x: sb.height / 2
    border { width: sb.activeFocus ? 1 : 0; color: activPal.highlight }
  }

  up.indicator: TipRect {
    x: sb.mirrored ? 0 : sb.width - sb.height
    implicitHeight: sb.height; implicitWidth: sb.height
    color: sb.enabled ? activPal.button : Qt.darker(disdPal.window, 1.3)
    rised: !sb.up.pressed
    scale: sb.up.pressed ? 0.9 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    Rectangle {
      x: parent.width / 4; width: parent.width / 2; height: parent.height / 15; y: parent.height * 0.48
      color: sb.enabled ? (sb.activeFocus ? activPal.text : activPal.dimText) : disdPal.text
    }
    Rectangle {
      x: parent.width / 4; width: parent.width / 2; height: parent.height / 15; y: parent.height * 0.48
      color: sb.enabled ? (sb.activeFocus ? activPal.text : activPal.dimText) : disdPal.text
      rotation: 90
    }
  }

  down.indicator: TipRect {
    x: sb.mirrored ? sb.width - sb.height : 0
    implicitHeight: sb.height; implicitWidth: sb.height
    color: sb.enabled ? activPal.button : Qt.darker(disdPal.window, 1.3)
    rised: !sb.down.pressed
    scale: sb.down.pressed ? 0.9 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    Rectangle {
      x: parent.width / 4; width: parent.width / 2; height: parent.height / 15; y: parent.height * 0.48
      color: sb.enabled ? (sb.activeFocus ? activPal.text : activPal.dimText) : disdPal.text
    }
  }
}
