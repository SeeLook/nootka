/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2020-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12


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
    property point startPos: Qt.point(0, 0)
    anchors.fill: parent
    hoverEnabled: true
    onEntered: whiteKey.entered(whiteKey)
    onPressed: startPos = Qt.point(mouseX, mouseY)
    onReleased: {
      var dx = mouseX - startPos.x
      var dy = mouseY - startPos.y
      if (Math.sqrt(dx * dx + dy * dy) < width * 2)
        whiteKey.clicked(whiteKey)
    }
    onExited: whiteKey.entered(null)
  }

  Component.onCompleted: {
    if (index % 7 !== 0 && index % 7 !== 3) {
      var bk = Qt.createComponent("qrc:/instruments/PianoKeyBlack.qml")
      black = bk.createObject(whiteKey, { "nr": index })
    }
  }
}
