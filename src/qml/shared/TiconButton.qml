/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


AbstractButton {
  id: root
  font.pixelSize: NOO.factor()
  focus: true

  property alias radius: bg.radius
  property alias color: bg.color
  property alias pixmap: img.source
  property alias iconHeight: img.sourceSize.height

  scale: GLOB.useAnimations && pressed ? 0.9 : 1.0
  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}

  implicitWidth: contentItem.implicitWidth + NOO.factor()

  contentItem: Item {
    implicitWidth: img.width + NOO.factor() / (NOO.isAndroid() ? 2 : 1) + txt.implicitWidth
    implicitHeight: img.height + NOO.factor() / (NOO.isAndroid() ? 4 : 2)
    Image {
      id: img
      x: Math.max(NOO.factor() / 2, (root.width - parent.implicitWidth) / 2)
      sourceSize.height: NOO.factor() * 2
      anchors.verticalCenter: parent.verticalCenter
      scale: root.activeFocus ? 1.1 : 1.0
    }
    Text {
      id: txt
      x: img.x + img.width + NOO.factor() / 3
      width: Math.min(implicitWidth, root.width - img.width - NOO.factor())
      font: root.font
      horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
      minimumPixelSize: 8; fontSizeMode: Text.HorizontalFit
      anchors.verticalCenter: parent.verticalCenter
      color: enabled ? (checked ? activPal.highlightedText : activPal.text) : disdPal.text
      text: root.text
      style: root.activeFocus ? Text.Sunken : Text.Normal
      styleColor: activPal.highlight
    }
  }

  background: GlowRect {
    id: bg
    color: enabled ? (root.checked ? activPal.highlight : activPal.button) : disdPal.button
    rised: !root.checked && !root.pressed
  }
}

