/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka 1.0
import "../"


TbandoneonBg {
  id: instrItem

  factor: Math.min(parent.height / 100, parent.width / 430)
  anchors.horizontalCenter: parent.horizontalCenter
  width: factor * 430
  height: parent.height

  // private
  property int hiId: -1
  property var correctAnim: null

  onCorrectInstrument: {
    if (!correctAnim) {
      var c = Qt.createComponent("qrc:/exam/CorrectInstrAnim.qml")
      correctAnim = c.createObject(instrItem)
    }
    correctAnim.doCross = wrongItem === null
    correctAnim.start()
  }

  Rectangle {
    visible: sideHighlight !== TbandoneonBg.HighlightNone
    color: GLOB.correctColor
    width: factor * 205; height: factor * 20
    x: factor / 2 + (sideHighlight === TbandoneonBg.HighlightRight ? factor * 225 : 0)
    y: parent.height - height
  }

   Image {
     source: Noo.pix("bando-bg")
     width: factor * 210; height: width * (sourceSize.height / sourceSize.width)
     x: -factor * 5
     z: 1
   }
   Image {
     source: Noo.pix("bando-bg")
     width: factor * 210; height: width * (sourceSize.height / sourceSize.width)
     x: factor * 225
     z: 2
   }

  Text {
    anchors.horizontalCenter: parent.horizontalCenter
    y: -10 * factor
    text: opening ? "\uE610" : (closing ? "\uE612" : "")
    color: opening ? "blue" : "#FFA500"
    font { family: "Scorek"; pixelSize: factor * 15 }
  }

  rightX: factor * 220// + buttonCol.width

  Row {
    id: mainRow
    height: factor * 100
    z: 5
    Item {
      height: parent.height; width: factor * 210
      Repeater {
        model: 33
        Image {
          source: Noo.pix("bando-button")
          sourceSize.width: Math.round(parent.height / 7.5)
          x: xAt(index) * factor
          y: yAt(index) * factor + width * 0.75
          MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: hiId = index
            onExited: hiId = -1
            onClicked: {
              if (!openButt.checked && !closeButt.checked)
                opening = true
              currentIndex = index
            }
          }
        }
      }
    }

    Column {
      id: buttonCol
      anchors.bottom: parent.bottom
      spacing: factor * 4
      bottomPadding: factor * 4

      TcuteButton {
        id: openButt
        width: factor * 10
        height: Noo.fontSize() * 3
        checked: opening
        checkable: true
        text: "\uE610"; font { family: "Scorek"; pixelSize: height }
        color: openButt.checked ? "blue" : "gray"
        onClicked: {
          opening = openButt.checked
          if (opening)
            closing = false
        }
      }
      TcuteButton {
        id: closeButt
        width: factor * 10
        height: Noo.fontSize() * 3
        checked: closing
        text: "\uE612"; font { family: "Scorek"; pixelSize: height }
        checkable: true
        color: closeButt.checked ? "#FFA500" : "gray"
        onClicked: {
          closing = closeButt.checked
          if (closing)
            opening = false
        }
      }
    }

    Item {
      height: parent.height; width: factor * 220
      Repeater {
        model: 38
        Image {
          source: Noo.pix("bando-button")
          sourceSize.width: Math.round(parent.height / 7.5)
          x: xAt(index + 33) * factor * 1.2
          y: yAt(index + 33) * factor + width * 0.75
          MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: hiId = index + 33
            onExited: hiId = -1
            onClicked: {
              if (!opening && !closing)
                opening = true
              currentIndex = index + 33
            }
          }
        }
      }
    }

  }

  Rectangle {
    id: hi
    color: GLOB.fingerColor
    width: factor * 12.5
    height: width
    radius: width / 2
    x: hiId > -1 ? (hiId > 32 ? rightX : 0) + xAt(hiId) * factor * (hiId > 32 ? 1.2 : 1) : 0
    y: hiId > -1 ? yAt(hiId) * factor + width * 0.825 : 0
    visible: hiId > -1
    z: 20
  }

  OutScaleTip { visible: outOfScale && !active }
}

