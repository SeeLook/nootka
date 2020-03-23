/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


Rectangle {
  id: whiteKey

  property int nr: -1
  property Item black: null

  signal entered(var key)
  signal clicked(var key)

  width: instrItem.keyWidth; height: instrItem.height;
  radius: width / 5; color: "white"
  border { width: Math.round(width / 16); color: "black" }

  MouseArea {
    id: ma
    anchors.fill: parent
    hoverEnabled: !instrItem.readOnly
    onEntered: whiteKey.entered(whiteKey)
    onClicked: whiteKey.clicked(whiteKey)
  }

  Component.onCompleted: {
    if (index % 7 !== 0 && index % 7 !== 3) {
      var bk = Qt.createComponent("qrc:/instruments/PianoKeyBlack.qml")
      black = bk.createObject(whiteKey, { "nr": index })
    }
  }
}
