/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

import Nootka 1.0


AbstractButton {
  id: root

  hoverEnabled: !Noo.isAndroid()

  width: Math.max(pix.width, butText.width) + factor * 2

  property alias pixmap: pix.source
  property alias name: butText.text
  property real factor: Noo.fontSize() * (Noo.isAndroid() ? 0.45 : 0.7)

  contentItem: Column {
    width: parent.width
    spacing: factor / 2
    TipRect {
      id: bg
      anchors.horizontalCenter: parent.horizontalCenter
      z: 1
      radius: Noo.fontSize() / 2; shadowRadius: Noo.fontSize()
      color: enabled ? (root.checked ? activPal.highlight : "#ffffff") : "#cbcbcb"
      shadowColor: "#101010"
      height: factor * 8; width: height
      Image {
        id: pix
        mipmap: true
        height: parent.height * 0.9; width: height * (sourceSize.width / sourceSize.height)
        anchors.centerIn: parent
        visible: enabled
      }
      Colorize {
        source: pix
        saturation: 0; hue: 0; lightness: 0
        anchors.fill: pix
        visible: !enabled
      }
      transformOrigin: Image.Center; scale: !enabled || pressed ? 0.8 : (GLOB.useAnimations && hovered ? 1.2 : 1.0)
      Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
      Behavior on color { enabled: GLOB.useAnimations; ColorAnimation { duration: 150 }}
    }

    Text {
      id: butText
      z: 0
      color: enabled ? (Noo.isAndroid() ? "#ffffff" : "#000000") : "#999999"
      font.pixelSize: Noo.fontSize() * (Noo.isAndroid() ? 0.8 : 1)
      horizontalAlignment: Text.AlignHCenter
      anchors.horizontalCenter: parent.horizontalCenter
    }
  }
}
