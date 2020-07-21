/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


TipRect {
  id: statusTip

  property alias text: text.text
  property int tipPos: Item.Top

  Connections {
    target: Noo
    onStatusTip: { text.text = statusText; statusTip.tipPos = tipPos }
  }

  x: 0.1 * nootkaWindow.width; y: statusTip.text === "" ? -1.2 * height : 0
  z: 500; radius: 0
  width: nootkaWindow.width * 0.8
  height: Math.min(nootkaWindow.height / 6, Noo.fontSize() * 7)

  color: Qt.tint(ma.pressed ? activPal.highlight : activPal.text, Noo.alpha(Noo.messageColor, 100))

  Text {
    id: text
    padding: statusTip.height / 10
    font.pixelSize: parent.height / 4
    anchors.fill: parent
    wrapMode: Text.WordWrap; verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
    color: ma.pressed ? activPal.highlightedText : activPal.base
  }

  MouseArea {
    id: ma
    anchors.fill: parent
    onReleased: Noo.setStatusTip("")
  }

  Behavior on y {
    enabled: GLOB.useAnimations
    SequentialAnimation {
      PauseAnimation { duration: 150 }
      NumberAnimation { easing.type: Easing.OutQuad; duration: 150 }
    }
  }
}
