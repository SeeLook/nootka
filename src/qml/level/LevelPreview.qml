/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka.exam 1.0


TlevelPreviewItem {
  id: levelPrev
  Rectangle {
    anchors.fill: parent
    color: activPal.base
    Text {
      color: Qt.tint(activPal.base, Noo.alpha(activPal.highlight, 50))
      font { family: "Nootka"; pixelSize: parent.height }
      anchors.centerIn: parent
      text: "h"
    }
  }

  Text {
    anchors.fill: parent
    horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
    text: levelHtml
  }
}
