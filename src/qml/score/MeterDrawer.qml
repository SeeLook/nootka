/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0
import "../"


Drawer { // meter menu
  visible: true
  width: nootkaWindow.width / 5; height: nootkaWindow.height
  background: TipRect { color: activPal.window; radius: 0 }

  // private
  property var colorArr: [ 0, 1, 1, 0 ]

  GridView {
    id: meterGrid
    anchors.fill: parent
    clip: true
    contentHeight: childrenRect.height
    cellHeight: Noo.fontSize() * 5
    cellWidth: parent.width / 2
    model: 12
    delegate: Rectangle {
      height: Noo.fontSize() * 5 - 2
      width: parent.width / 2 - 2
      color: score.scoreObj.meter === Math.pow(2, index) ? activPal.highlight :
              (area.containsMouse ? Qt.tint(activPal.base, Noo.alpha(activPal.highlight, 50)) : (colorArr[index % 4] === 1 ? activPal.alternateBase : activPal.base))
      Text {
        id: buttText
        anchors.horizontalCenter: parent.horizontalCenter
        y: -Noo.fontSize() * 4.5
        font { family: "Scorek"; pixelSize: Noo.fontSize() * 4 }
        text: Noo.meter(Math.pow(2, index)).symbol()
        color: score.scoreObj.meter === Math.pow(2, index) ? activPal.highlightedText : activPal.text
      }
      MouseArea {
        anchors.fill: parent
        id: area
        hoverEnabled: true
        onClicked: {
          score.scoreObj.setMeter(Math.pow(2, index))
          meter.text = buttText.text
          close()
        }
      }
    }
  }
}
