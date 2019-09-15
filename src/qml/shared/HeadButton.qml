/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import Nootka 1.0


ToolButton {
  id: root

  hoverEnabled: true

  width: Math.max(pix.width, butText.width) + (Noo.isAndroid() ? 4 : factor * 2)
  height: butText.height + pix.height + Screen.pixelDensity * 2

  property alias pixmap: pix.source
  property alias name: butText.text
  property real factor: nootkaWindow.width / 240 //Math.min(nootkaWindow.height / 140, nootkaWindow.width / 220)
  property alias fontSize: butText.font.pixelSize
  property alias textColor: butText.color
  property Taction taction
  property bool hiHover: true

  background: Item {} // Rectangle {
//     color: Noo.alpha(activPal.highlight, 50)
//     scale: hovered && hiHover ? 1 : 0
//     Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
//   }

  onHoveredChanged: {
    if (GLOB.showHints && taction && taction.tip !== "") {
      if (hovered)
        Noo.setStatusTip(taction.tip, taction.tipPos)
      else
        Noo.setStatusTip("", taction.tipPos)
    }
  }

  onClicked: {
    if (taction)
      taction.trigger()
    focus = false
  }

  Text {
    id: butText
    text: taction ? taction.text : ""
    font.pixelSize: Math.min(factor * 2.5, Noo.fontSize())
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    horizontalAlignment: Text.AlignHCenter
    color: activPal.text
  }

  Image {
    id: pix
    mipmap: true
    source: taction ? taction.icon : ""
    y: Screen.pixelDensity + (GLOB.useAnimations && !pressed && hiHover && hovered ? (root.height - height - Screen.pixelDensity) / 2 : 0)
    height: factor * 8; width: height * (sourceSize.width / sourceSize.height)
    anchors.horizontalCenter: butText.horizontalCenter
    transformOrigin: Image.Center; scale: pressed ? 0.9 : (GLOB.useAnimations && hiHover && hovered ? root.height / height : 1.0)
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    Behavior on y { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  }

  Connections {
    target: taction
    onShakeButton: shakeAnim.running = true
  }

  SequentialAnimation {
    id: shakeAnim
    running: false
    loops: 3
    NumberAnimation { target: root; property: "rotation"; from: 0; to: -30; duration: 50 }
    NumberAnimation { target: root; property: "rotation"; from: -30; to: 30; duration: 100 }
    NumberAnimation { target: root; property: "rotation"; from: 30; to: 0; duration: 50 }
  }
}
