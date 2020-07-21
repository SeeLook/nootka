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

  x: (tipPos === Item.Top ? 0.3 : (tipPos === Item.TopRight ? 0.6 : 0)) * nootkaWindow.width
  y: statusTip.text === "" ? -1.2 * height : 0
  z: 500
  radius: 0
  width: nootkaWindow.width * 0.4
  height: nootkaWindow.header.height

  color: Qt.tint(activPal.text, Noo.alpha(Noo.messageColor, 100))

  Text {
    id: text
    padding: statusTip.height / 10
    font.pixelSize: nootkaWindow.height / 50
    anchors.fill: parent
    wrapMode: Text.WordWrap; verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
    color: activPal.base
  }

  Behavior on y {
    enabled: GLOB.useAnimations
    SequentialAnimation {
      PauseAnimation { duration: 150 }
      NumberAnimation { easing.type: Easing.OutQuad; duration: 150 }
    }
  }
}
