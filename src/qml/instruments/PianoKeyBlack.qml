/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12


Image {
  id: blackKey

  property int nr: -1

  source: NOO.pix("pianokey")
  width: Math.round(parent.width * 0.8); height: parent.height / 2
  x: -parent.width * 0.4; y: NOO.factor() * (NOO.isAndroid() ? 1 : 1.5)
  z: 3

  MouseArea {
    id: ma
    anchors.fill: parent
    hoverEnabled: true
    onEntered: whiteKey.entered(blackKey)
    onClicked: whiteKey.clicked(blackKey)
    onExited: whiteKey.entered(null)
  }
}
