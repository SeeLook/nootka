/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import "../"


GlowRect {
  property bool show: false

  scale: show ? 1 : 0
  width: parent.width / 4; height: NOO.factor() * (2 + txt.lineCount * 1.5)
  anchors.centerIn: parent
  z: 100
  color: Qt.tint("red", NOO.alpha(activPal.base, 70))
  shadowRadius: NOO.factor()

  Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}

  Text {
    id: txt
    anchors.centerIn: parent
    width: parent.width * 0.9
    horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
    text: qsTr("Out of instrument scale!")
    font.pixelSize: NOO.factor() * 1.5
    wrapMode: Text.WordWrap
    fontSizeMode: Text.Fit; minimumPixelSize: NOO.factor() / 2; minimumPointSize: minimumPixelSize
    elide: Text.ElideRight
  }
}
