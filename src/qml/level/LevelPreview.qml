/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka.Dialogs 1.0
import "../"


TlevelPreviewItem {
  id: levelPrev
  Rectangle {
    anchors.fill: parent
    color: activPal.base
    Text {
      color: Qt.tint(activPal.base, Noo.alpha(activPal.highlight, 50))
      font { family: "Nootka"; pixelSize: parent.width * 0.75 }
      anchors.centerIn: parent
      text: instrumentGlyph
    }
  }

  Tflickable {
    anchors.fill: parent
    contentWidth: width
    contentHeight: htmlText.height
    Text {
      id: htmlText
      property int pixelSize: Noo.fontSize()
      padding: Noo.fontSize() / 3
      width: parent.width
      horizontalAlignment: Text.AlignHCenter
      textFormat: Text.RichText
      text: levelHtml
      color: activPal.text
      wrapMode: Text.WordWrap
      font.pixelSize: pixelSize
    }
    MouseArea {
      anchors.fill: parent
      z: -1
      onWheel: {
        if (wheel.modifiers & Qt.ControlModifier) {
            if (wheel.angleDelta.y > 0) {
                if (!zoomTimer.running) {
                  htmlText.pixelSize = htmlText.pixelSize + 1
                  zoomTimer.running = true
                }
            } else if (wheel.angleDelta.y < 0) {
                if (!zoomTimer.running) {
                  htmlText.pixelSize = htmlText.pixelSize - 1
                  zoomTimer.running = true
                }
            }
        } else
            wheel.accepted = false
      }
    }
    Timer { id: zoomTimer; interval: 100 }
  }
}
