/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Rectangle {
  id: root
  width: parent.width; height: text.height + Noo.fontSize() / 2
  gradient: Gradient{
    GradientStop { position: 0.0; color: Qt.lighter(activPal.highlight) }
    GradientStop { position: 1.0; color: activPal.highlight }
  }

  property alias textColor: text.color
  property alias font: text.font
  property string text: ""

  Text {
    id: text
    font { pixelSize: Noo.fontSize() * 1.3; bold: true }
    anchors.centerIn: parent
    text: root.text
    textFormat: Text.StyledText
    color: activPal.highlightedText
  }
}
