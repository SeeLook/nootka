/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Rectangle {
  id: cb

  property real factor: 1
  property bool selected: false
  property alias text: txt.text
  property alias font: txt.font
  property alias textColor: txt.color
  property alias yOffset: txt.y

  signal entered()
  signal exited()
  signal clicked()

  color: selected ? activPal.highlight : "transparent"
  width: factor * 2
  height: factor * 3
  radius: factor / 5

  Text {
    id: txt
    x: (factor * 2 - width) / 2
    height: factor * 3
    style: Text.Normal
    styleColor: selected ? activPal.highlightedText : activPal.shadow
    color: activPal.text
  }
  MouseArea {
    anchors.fill: parent
    hoverEnabled: true
    onClicked: cb.clicked()
    onEntered: { txt.style = Text.Outline;  cb.entered() }
    onExited: { txt.style = Text.Normal; cb.exited() }
  }
}
