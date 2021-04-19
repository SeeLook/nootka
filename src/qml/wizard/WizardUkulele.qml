/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


Column {
  id: saxCol

  spacing: NOO.factor()
  anchors.horizontalCenter: parent.horizontalCenter
  width: parent.width - NOO.factor() * 2

  Text {
    width: parent.width
    font { pixelSize: NOO.factor() * 1.2; bold: true }
    wrapMode: Text.WordWrap
    horizontalAlignment: Text.AlignHCenter
    color: activPal.text
    text: "Some initial words about ukulele support in Nootka"
  }
}
