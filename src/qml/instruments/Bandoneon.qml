/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka 1.0
import "../"


TbandoneonBg {
  id: instrItem

  width: Math.max(factor * 480, nootkaWindow.width)
  height: GLOB.instrument.getItemHeight(nootkaWindow.height)
  factor: height / 100

  // private
  property int hiId: -1
  property var correctAnim: null
  property var bandoZoom: null

  transformOrigin: Item.TopLeft
  Behavior on scale {
    enabled: GLOB.useAnimations
    NumberAnimation {
      duration: 150
      onRunningChanged:  {
        if (bandoZoom && !running && scale > 1)
          instrFlick.contentX = bandoZoom.flickX
      }
    }
  }

  onCorrectInstrument: {
    if (!correctAnim)
      correctAnim = Qt.createComponent("qrc:/exam/CorrectInstrAnim.qml").createObject(instrItem)
    correctAnim.doCross = wrongItem === null
    correctAnim.start()
  }

  Rectangle {
    visible: sideHighlight !== TbandoneonBg.HighlightNone
    color: GLOB.correctColor
    width: factor * 207; height: factor * 20
    x: factor / 2 + (sideHighlight === TbandoneonBg.HighlightRight ? factor * 225 : 0)
    y: parent.height - height
  }

   Image {
     source: NOO.pix("bando-bg")
     width: factor * 220; height: width * (sourceSize.height / sourceSize.width)
     x: mainRow.x
     z: 1
   }
   Image {
     source: NOO.pix("bando-bg")
     width: factor * 220; height: width * (sourceSize.height / sourceSize.width)
     x: mainRow.x + factor * 240
     z: 2
   }

  Text {
    x: (parent.width * 0.985 - width) / 2
    y: factor * 2
    text: opening ? "\uE610" : (closing ? "\uE612" : "")
    color: opening ? "blue" : "#FF00FF"
    font { family: "Nootka"; pixelSize: factor * 25 }
  }

  rightX: factor * 240
  xOffset: mainRow.x

  Row {
    id: mainRow
    anchors.horizontalCenter: parent.horizontalCenter
    height: factor * 100
    z: 5
    Item {
      height: parent.height; width: factor * 220
      Repeater {
        model: 33
        Image {
          source: NOO.pix("bando-button")
          sourceSize.width: Math.round(parent.height / 6.5)
          x: xAt(index) * factor * 1.1
          y: yAt(index) * factor * 1.2 + width * 0.25
          MouseArea {
            anchors.fill: parent
            hoverEnabled: !NOO.isAndroid()
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
        width: factor * 20; height: factor * 15
        checked: opening
        checkable: true
        text: "\uE610"; font { family: "Nootka"; pixelSize: height }
        color: openButt.checked ? "blue" : "gray"
        hoverEnabled: !NOO.isAndroid()
        onHoveredChanged: {
          if (hovered)
            NOO.setStatusTip(qsTr("Bellows is opening.",
                "Check please what bandoneon/accordion bellows does in your language. It may be more sophisticated word than 'opening'"),
                Item.Top)
          else
            NOO.setStatusTip("", Item.Top)
        }
        onClicked: {
          opening = openButt.checked
          if (opening)
            closing = false
        }
      }
      TcuteButton {
        id: closeButt
        width: factor * 20; height: factor * 15
        checked: closing
        text: "\uE612"; font { family: "Nootka"; pixelSize: height }
        checkable: true
        color: closeButt.checked ? "#FF00FF" : "gray"
        hoverEnabled: !NOO.isAndroid()
        onHoveredChanged: {
          if (hovered)
            NOO.setStatusTip(qsTr("Bellows is closing.",
                "Check please what bandoneon/accordion bellows does in your language. It may be more sophisticated word than 'closing'"),
                Item.Top)
          else
            NOO.setStatusTip("", Item.Top)
        }
        onClicked: {
          closing = closeButt.checked
          if (closing)
            opening = false
        }
      }
    }

    Item {
      height: parent.height; width: factor * 230
      Repeater {
        model: 38
        Image {
          source: NOO.pix("bando-button")
          sourceSize.width: Math.round(parent.height / 6.5)
          x: xAt(index + 33) * factor * 1.3 - factor * 10
          y: yAt(index + 33) * factor * 1.05 + width * 0.45
          MouseArea {
            anchors.fill: parent
            hoverEnabled: !NOO.isAndroid()
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
    width: factor * 14.5; height: width
    radius: width / 2
    x: mainRow.x + (hiId > -1 ? (hiId > 32 ? rightX - factor * 10 : 0) + xAt(hiId) * factor * (hiId > 32 ? 1.3 : 1.1) : 0)
    y: hiId > -1 ? yAt(hiId) * factor * (hiId > 32 ? 1.05 : 1.2) + width * ((hiId > 32 ? 0.48 : 0.28)) : 0
    visible: hiId > -1
    z: 20
  }

  Component.onCompleted: {
    if (NOO.isAndroid() && NOO.fingerPixels() * 4 > height)
      bandoZoom = Qt.createComponent("qrc:/instruments/InstrumentZoom.qml").createObject(instrItem)
  }

  OutScaleTip { show: !active && outOfScale }
}

