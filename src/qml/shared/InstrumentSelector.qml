/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0

// import Nootka 1.0


Flow {
  id: instrSel

  property int instrNr: 0
  property var instrGlyphs: ["v", "h", "j", "i", "f"]

  spacing: nootkaWindow.fontSize


  Item {
    width: nootkaWindow.fontSize * 15
    height: parent.height
    Text {
      id: instrText
      text: Noo.instrumentName(instrNr)
      anchors.centerIn: parent
      font { pixelSize: nootkaWindow.fontSize * 1.5; bold: true }
      Behavior on text {
        enabled: GLOB.useAnimations;
        SequentialAnimation {
            NumberAnimation { target: instrText; property: "opacity"; to: 0 }
            PropertyAction {}
            NumberAnimation { target: instrText; property: "opacity"; to: 1 }
        }
      }
    }
  }

  Repeater {
    model: instrGlyphs.length
    Text {
      id: iText
      font {family: "nootka"; pixelSize: 60 }
      text: instrGlyphs[index]
      color: instrNr === index ? activPal.highlight : activPal.text
      scale: instrNr === index ? 1.4 : 1.0
      y: instrNr === index ? -nootkaWindow.fontSize / 3 : 0
      Behavior on y { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.1; duration: 500 }}
      Behavior on scale { enabled: GLOB.useAnimations; PropertyAnimation { duration: 300 }}
      MouseArea {
        anchors.fill: parent
        onClicked: {
          instrNr = index
        }
      }
    }
  }
}
