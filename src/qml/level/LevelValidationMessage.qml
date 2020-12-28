/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


TpopupDialog {
  property alias message: lv.model
  property alias caption: hText.text
  property color accent: activPal.highlight

  bgColor: Qt.tint(activPal.base, Noo.alpha(accent, 20))
  border { color: accent; width: Noo.fontSize() / 4.0 }
  visible: true; modal: true
  width: lv.width + Noo.fontSize() / 2
  height: lv.height + header.height + footer.height + Noo.fontSize()

  rejectButton.visible: false
  acceptButton.text: Noo.TR("QPlatformTheme", "OK")
  acceptButton.visible: !Noo.isAndroid()

  header: Rectangle {
    color: "transparent"; width: parent.width; height: hText.height + Noo.fontSize() / 2; radius: Noo.fontSize() / 4
    Text {
      id: hText
      width: parent.width - Noo.fontSize()
      fontSizeMode: Text.HorizontalFit
      anchors.centerIn: parent; horizontalAlignment: Text.AlignHCenter
      color: activPal.text
      font { pixelSize: Noo.fontSize() * 1.5; bold: true }
    }
    Rectangle { width: parent.width; height: 1; color: accent; y: parent.height }
  }

  ListView {
    id: lv
    width: Noo.fontSize() * 40; height: contentHeight
    spacing: 2; clip: true
    delegate: Rectangle {
      width: lv.width; height: mText.height + Noo.fontSize() / 2
      color: index % 2 === 1 ? "transparent" : Noo.alpha(activPal.base, 50)
      Rectangle {
        width: Noo.fontSize() / 2; height: width; radius: width / 2
        x: Noo.fontSize() / 2; y: (parent.height - height) / 2
        color: activPal.text
      }
      Text {
        id: mText
        x: Noo.fontSize() * 1.5; y: (parent.height - height) / 2
        width: lv.width - Noo.fontSize() * 2
        wrapMode: Text.WordWrap
        text: modelData
        font.pixelSize: Noo.fontSize()
      }
    }
  }

  onClosed: destroy()
}
