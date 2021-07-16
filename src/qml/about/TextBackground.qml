/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import "../"

Text {
    font { pixelSize: NOO.factor() * 1.6; bold: true }
    anchors.horizontalCenter: parent.horizontalCenter
    textFormat: Text.StyledText
    color: activPal.highlight

    Rectangle {
      x: (parent.width - width + NOO.factor()) / 2
      y: NOO.factor() * 2
      color: activPal.highlight
      width: authorsPage.width - NOO.factor(); height: NOO.factor() / 12
    }
}
