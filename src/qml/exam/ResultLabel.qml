/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Rectangle {
  property string label
  property alias score: result.text

  height: Noo.fontSize() * 1.5; width: Noo.fontSize() * 3
  color: resultArea.containsMouse ? activPal.highlight : activPal.mid
  z: 15

  Text {
    id: result
    anchors.fill: parent
    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
    color: resultArea.containsMouse ? activPal.highlightedText : activPal.text
  }

  MouseArea {
    id: resultArea
    anchors.fill: parent
    hoverEnabled: true
    onClicked: console.log("hjdhfajfhajshfaj")
  }
}
