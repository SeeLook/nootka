/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import "../"


TipRect {
  width: visible ? parent.width / 4 : 0; height: Noo.fontSize() * (2 + txt.lineCount * 1.5)
  x: (parent.width - width) / 2
  y: visible ? (parent.height - height) / 2 : parent.height
  z: 100
  color: Qt.tint("red", Noo.alpha(activPal.base, 80))
  shadowRadius: Noo.fontSize()
  Text {
    id: txt
    width: parent.width * 0.9
    anchors.centerIn: parent
    text: qsTr("Out of instrument scale!")
    font.pixelSize: Noo.fontSize() * 1.5
    wrapMode: Text.WordWrap
    horizontalAlignment: Text.AlignHCenter
  }
}
