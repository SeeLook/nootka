/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0


ListView {
  id: clefMenu
  width: parent.width; height: parent.height
  clip: true
  spacing: 1

  property int selClef
  // private
  property var clefNr: [0, 1, 2, 4, 8, 32, 128]

  signal clicked(var cl)

  function checkClef() {
    for (var c = 0; c < 7; ++c) { // so far seven clefs are supported
      if (clefNr[c] === selClef)
        clefMenu.currentIndex = c
    }
  }

  onSelClefChanged: checkClef()

  onVisibleChanged: {
    if (visible)
      checkClef()
  }

  ScrollBar.vertical: ScrollBar { active: false; visible: active }

  model: 7
  delegate: Rectangle {
    width: clefMenu.width
    height: visible ? Noo.fontSize() * (index === 6 ? 10 : 7.5) : 0
    visible: index !== 0 || score.meter !== Tmeter.NoMeter
    color: index === clefMenu.currentIndex ? activPal.highlight :
                  (area.containsMouse ? Qt.tint(activPal.base, Noo.alpha(activPal.highlight, 50)) : (index % 2 === 1 ? activPal.alternateBase : activPal.base))
    Row {
      height: parent.height
      width: parent.width
      spacing: Noo.fontSize()
      Item {
        width: Noo.fontSize() * 3.2; height: Noo.fontSize()
        Text {
          scale: Noo.fontSize() * 0.9
          x: scale * 2.5; y: 4.6 * Noo.fontSize()
          font { family: "scorek"; pixelSize: 3 }
          text: Noo.clef(clefNr[index]).glyphOnStaff()
          color: index === clefMenu.currentIndex ? activPal.highlightedText : activPal.text
        }
      }
      Column {
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width - Noo.fontSize() * 4.8
        spacing: Noo.fontSize() / 4
        Text {
          text: Noo.clef(clefNr[index]).name()
          font { bold: true; pixelSize: Noo.fontSize() * 1.1 }
          color: index === clefMenu.currentIndex ? activPal.highlightedText : activPal.text
        }
        Text {
          text: Noo.clef(clefNr[index]).desc()
          font { pixelSize: Noo.fontSize() * 0.8 }
          width: parent.width
          wrapMode: Text.WordWrap
          color: index === clefMenu.currentIndex ? activPal.highlightedText : activPal.text
        }
      }
    }
    MouseArea {
      anchors.fill: parent
      id: area
      hoverEnabled: true
      onClicked: {
        selClef = clefNr[index]
        clefMenu.currentIndex = index
        clefMenu.clicked(clefNr[index])
      }
    }
  }
}
