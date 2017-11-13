/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0


Drawer { // meter menu
  visible: true
  width: nootkaWindow.width / 4; height: nootkaWindow.height
  background: Rectangle { color: activPal.window }
  
  Flickable {
    anchors.fill: parent
    clip: true
    contentHeight: childrenRect.height
    Grid {
      width: parent.width
      leftPadding: Noo.fontSize() / 4
      topPadding: Noo.fontSize() / 2
      columns: 2
      spacing: Noo.fontSize() / 2
      
      Repeater {
        model: 12
        TcuteButton {
          height: Noo.fontSize() * 5
          width: parent.width / 2.1
          color: checked || pressed ? activPal.highlight : activPal.button
          checkable: true
          checked: score.scoreObj.meter === Math.pow(2, index)
          Text {
            id: buttText
            anchors.horizontalCenter: parent.horizontalCenter
            y: -Noo.fontSize() * 3.5
            font { family: "Scorek"; pixelSize: Noo.fontSize() * 4 }
            text: Noo.meter(Math.pow(2, index)).symbol()
            color: pressed ? activPal.highlightedText : activPal.text
          }
          onClicked: {
            score.scoreObj.setMeter(Math.pow(2, index))
            meter.text = buttText.text
            close()
          }
        }
      }
    }
  }
}
