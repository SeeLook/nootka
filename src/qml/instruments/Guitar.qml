/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtGraphicalEffects 1.0

import Nootka 1.0


Item {
  id: root
  anchors.fill: parent

  TguitarBg {
    id: guitar
    anchors.fill: parent
  }

  Image {
    id: body
    cache: false
    source: Tpath.pix("body")
    fillMode: Image.PreserveAspectFit
    height: parent.height * 4
    x: guitar.xiiFret
    y: parent.height - height * 0.95
    z: -1
  }

  Image {
    id: rosette
    cache: false
    source: Tpath.pix("rosette")
    fillMode: Image.PreserveAspectFit
    height: parent.height * 1.5
    x: guitar.fbRect.width - height * 0.2
    y: parent.height - height * 0.95
    z: -1
  }

  Rectangle {
    id: finger
    color: Qt.rgba(1, 0, 0.5, 0.78)
    width: guitar.fretWidth / 1.6
    height: width * 0.75
    radius: width / 2
    x: guitar.fingerPos.x;
    y: guitar.fingerPos.y - height * 0.25
    visible: false
  }

  DropShadow {
    id: fingerShadow
    anchors.fill: finger
    horizontalOffset: guitar.stringsGap / 7
    verticalOffset: guitar.stringsGap / 7
    color: activPal.shadow
    radius: 8.0
    source: finger
    visible: guitar.active && guitar.fingerPos.x > 0
  }

  Rectangle {
    id: string
    color: finger.color
    width: guitar.width - guitar.stringsGap - 2
    height: guitar.string < 6 ? guitar.strWidth(guitar.string) * 1.5 : 0
    x: 1
    y: guitar.string < 6 ? 10 + guitar.stringsGap / 2 + guitar.string * guitar.stringsGap - height / 3 : 0
    visible: guitar.active && guitar.string < 6 && guitar.fingerPos.x == 0
  }
}
