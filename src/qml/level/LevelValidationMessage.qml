/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


TpopupDialog {
  property alias message: lv.model
  property color accent: activPal.highlight

  bgColor: Qt.tint(activPal.base, NOO.alpha(accent, 20))
  border { color: accent; width: NOO.factor() / 4.0 }
  visible: true; modal: true
  width: lv.width + NOO.factor() / 2
  height: lv.height + header.height + footer.height + NOO.factor()
  glowRect.radius: NOO.factor()

  rejectButton.visible: false
  acceptButton.text: NOO.TR("QPlatformTheme", "OK")
  acceptButton.visible: !NOO.isAndroid()

  ListView {
    id: lv
    width: NOO.factor() * 40; height: contentHeight
    spacing: 2; clip: true
    delegate: Rectangle {
      width: lv.width; height: mText.height + NOO.factor() / 2
      color: index % 2 === 1 ? "transparent" : NOO.alpha(activPal.base, 50)
      Rectangle {
        width: NOO.factor() / 2; height: width; radius: width / 2
        x: NOO.factor() / 2; y: (parent.height - height) / 2
        color: activPal.text
      }
      Text {
        id: mText
        x: NOO.factor() * 1.5; y: (parent.height - height) / 2
        width: lv.width - NOO.factor() * 2
        wrapMode: Text.WordWrap
        text: modelData
        font.pixelSize: NOO.factor()
      }
    }
  }

  onClosed: destroy()
}
