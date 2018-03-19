/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Rectangle {
  id: statusTip

  property alias text: text.text
  property int tipPos: Item.Top

  Connections {
    target: Noo
    onStatusTip: { text.text = statusText; statusTip.tipPos = tipPos }
  }

  color: activPal.base
  width: nootkaWindow.width * 0.4
  height: Noo.isAndroid() ? nootkaWindow.height / 9 : nootkaWindow.header.height
  scale: text.text ? 1 : 0
  z: 1000
  radius: height / 10

  x: (tipPos === Item.Top ? 0.3 : (tipPos === Item.TopRight ? 0.6 : 0)) * nootkaWindow.width

  Text {
    id: text
    font.pixelSize: nootkaWindow.height / 45
    anchors.fill: parent
    wrapMode: Text.WordWrap; verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
    color: activPal.text
  }

  Behavior on scale { enabled: GLOB.useAnimations;
    SequentialAnimation {
      PauseAnimation { duration: 150 }
      NumberAnimation {}
    }
  }
}
