/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"

Tile {
  bgColor: activPal.highlight

  property alias textColor: text.color
  property alias font: text.font
  property alias text: text.text

  Text {
    id: text
    font { pixelSize: Noo.factor() * 1.3; bold: true }
    anchors.horizontalCenter: parent.horizontalCenter
    textFormat: Text.StyledText
    color: activPal.highlightedText
  }
}
