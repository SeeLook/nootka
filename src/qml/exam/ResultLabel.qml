/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Rectangle {
  id: label
  property alias score: result.text
  property color bg: activPal.button
  property int index: -1
  property bool hi: false

  height: nootkaWindow.width / 45; width: height * 2
  color: resultArea.containsMouse || hi ? activPal.highlight : Qt.tint(activPal.mid, Noo.alpha(bg, 40))
  z: 15

  Text {
    id: result
    font.pixelSize: parent.height * 0.75
    anchors.fill: parent
    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
    color: resultArea.containsMouse || hi ? activPal.highlightedText : activPal.buttonText
  }

  MouseArea {
    id: resultArea
    anchors.fill: parent
    hoverEnabled: true
    onClicked: more()
  }

  function more() {
    resultsItem.more(label)
  }
}
