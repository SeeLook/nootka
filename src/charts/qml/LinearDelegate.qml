/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka.Charts 1.0


TlinChartDelegate {
  id: lineDel

  Rectangle { // average line
    visible: averageY > 0.0
    color: "#00a0a0"
    x: width / 2; z: 1
    y: maxDataHeight * 0.1 + maxDataHeight - (averageY / chartItem.maxYValue()) * maxDataHeight - height / 2
    width: parent.width; height: lThick * 1.3
    MouseArea {
      anchors.fill: parent
      hoverEnabled: true
      onEntered: lineEntered()
      onExited: lineExited()
    }
  }
  Rectangle {
    color: activPal.text
    y: parent.height - width + lThick / 2
    width: parent.width; height: lThick
    Rectangle {
      color: activPal.text
      x: parent.width - lThick; y: lThick
      width: lThick; height: lThick * 2
      Column {
        y: lThick * 1.5
        anchors.horizontalCenter: parent.horizontalCenter
        Text { // question number
          text: nrText; font { pixelSize: lineDel.width / 6 }
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.StyledText; horizontalAlignment: Text.AlignHCenter
        }
        Item { // note name
          width: childrenRect.width; height: lineDel.width / 4
          anchors.horizontalCenter: parent.horizontalCenter
          Text {
            y: -lineDel.width / 4
            text: noteText; font { pixelSize: lineDel.width / 5; family: "Scorek" }
            color: activPal.text; textFormat: Text.StyledText
          }
        }
        Text { // position on instrument
          text: posText; font { pixelSize: lineDel.width / 6; bold: true }
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.PlainText
        }
        Text { // key signature
          text: keyText; font { pixelSize: lineDel.width / 6; italic: true }
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.PlainText
        }
      }
    }
  }

  Text {
    id: nText
    color: pointColor; text: pointSymbol
    y: pointY - (pointSymbol === "G" ? height * 0.8 : height * 0.1)
    x: parent.width - (pointSymbol === "G" ? width / 3 : width * 0.5)
    font { pixelSize: lineDel.width / 2; family: "Nootka" }
    visible: false
  }
  DropShadow {
    z: 5010
    anchors.fill: nText
    horizontalOffset: lineDel.width / 35
    verticalOffset: horizontalOffset
    radius: Noo.fontSize()
    samples: 1 + radius * 2
    color: activPal.shadow
    source: nText
    scale: ma.containsMouse ? 1.4 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    MouseArea {
      id: ma
      anchors.fill: parent
      hoverEnabled: true
      onEntered: pointEntered()
      onExited: pointExited()
    }
  }
}
