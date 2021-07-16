/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12


Dialog {
  id: dialog

  property alias bgColor: glowRect.color
  property alias shadowColor: glowRect.shadowColor
  property alias rejectButton: rejectButton
  property alias acceptButton: acceptButton
  property alias border: glowRect.border
  property alias caption: hText.text
  property alias glowRect: glowRect
  property alias footWidth: butRow.width

  width: parent.width * 0.8; height: parent.height * 0.8
  x: (parent.width - width) / 2; y: (parent.height - height) / 2
  background: GlowRect { id: glowRect; color: activPal.window; shadowColor: activPal.shadow; radius: NOO.factor() }
  scale: GLOB.useAnimations ? 0 : 1.0
  enter: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 1.0 }}
  exit: Transition { enabled: GLOB.useAnimations; NumberAnimation { property: "scale"; to: 0 }}

  header: Rectangle {
    color: "transparent"
    width: parent.width; height: NOO.factor() * 2.2; radius: NOO.factor() / 4
    visible: hText.text !== ""
    Text {
      id: hText
      width: parent.width - NOO.factor()
      fontSizeMode: Text.HorizontalFit
      anchors.centerIn: parent; horizontalAlignment: Text.AlignHCenter
      color: activPal.text
      font { pixelSize: NOO.factor() * 1.5; bold: true }
    }
    Rectangle { width: parent.width; height: 1; color: glowRect.border.color; y: parent.height }
  }

  footer: Rectangle {
    color: "transparent"; width: parent ? parent.width : 0; height: butRow.height; radius: NOO.factor() / 4
    Row {
      id: butRow; spacing: parent.width / 10; padding: parent.height / 7
      anchors.horizontalCenter: parent.horizontalCenter
      TiconButton {
        id: rejectButton
        pixmap: NOO.pix("exit"); text: NOO.TR("QShortcut", "Cancel")
        onClicked: reject()
        color: Qt.tint(activPal.button, NOO.alpha("red", NOO.isAndroid() ? 40 : 0))
      }
      TiconButton {
        id: acceptButton
        pixmap: NOO.pix("check"); text: NOO.TR("QPlatformTheme", "Apply")
        onClicked: accept()
        color: Qt.tint(activPal.button, NOO.alpha("green", NOO.isAndroid() ? 40 : 0))
      }
    }
  }
}
