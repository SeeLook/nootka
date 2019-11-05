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
    color: chartItem.averLineGroup === groupNr ? "#00c0c0" : (groupNr % 2 ? "#008080" : "#00a0a0")
    x: width / 2; z: 1
    y: parent.height * 0.1 + maxDataHeight - (averageY / chartItem.maxYValue()) * maxDataHeight - height / 2
    width: parent.width; height: lThick / 2
    MouseArea {
      id: averMa
      width: parent.width; height: parent.height * 5; y: -2 * parent.height
      hoverEnabled: true
      onEntered: lineEntered(mouseX, parent.y)
      onExited: lineExited()
    }
  }

  Rectangle { // X axis line (partial)
    color: activPal.text
    y: parent.height - width + lThick / 2
    width: parent.width; height: lThick
    Rectangle { // X tick
      color: activPal.text
      x: parent.width - lThick; y: lThick
      width: lThick; height: lThick * 2
      Column { // X value (description)
        y: lThick * 2
        anchors.horizontalCenter: parent.horizontalCenter
        Text { // question number
          text: nrText; font { pixelSize: lineDel.width / 7 }
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.StyledText; horizontalAlignment: Text.AlignHCenter
        }
        Item { // note name
          width: childrenRect.width; height: lineDel.width / 4
          anchors.horizontalCenter: parent.horizontalCenter
          Text {
            y: -lineDel.width / (nrText === "" ? 5 : 4.5)
            text: noteText; font { pixelSize: lineDel.width / 5; family: "Scorek" }
            color: activPal.text; textFormat: Text.StyledText
          }
        }
        Text { // position on instrument
          text: posText; font { pixelSize: lineDel.width / 6 }
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.RichText
        }
        Text { // key signature
          text: keyText; font { pixelSize: lineDel.width / 7; italic: true }
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.PlainText
        }
      }
    }
  }

  Text {
    id: nText
    color: pointColor; text: pointSymbol
    y: pointY - (pointSymbol === "M" ? height * 0.3 : height * 0.8)
    x: parent.width - (pointSymbol === "M" ? width * 0.5 : width / 3)
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

  Component {
    id: hintComp
    Text {
      z: -1
      x: (parent.width - width) / 2; y: parent.height * 0.05
      color: Noo.alpha(activPal.text, 75)
      textFormat: Text.RichText
    }
  }

  property var hintText: null

  onNrChanged: {
    if (groupNr > -1) {
      var h = getHintText()
      if (h !== "") {
          if (!hintText)
            hintText = hintComp.createObject(lineDel)
          hintText.text = h
      } else {
        if (hintText)
          hintText.destroy()
      }
    }
  }
}
