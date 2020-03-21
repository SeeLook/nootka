/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka 1.0


TpianoBg {
  id: instrItem

  anchors.fill: parent

  keyWidth: height / (Noo.isAndroid() ? 5.5 : 5)

  onWantKeyToSelect: selectKey(k > -1 ? (isWhite ? whiteRep.itemAt(k) : whiteRep.itemAt(k).black) : null)

//   private
  property var activeKey: null
  property var correctAnim: null

  onCorrectInstrument: {
    if (!correctAnim) {
      var c = Qt.createComponent("qrc:/exam/CorrectInstrAnim.qml")
      correctAnim = c.createObject(instrItem)
    }
    correctAnim.doCross = !wrongItem.parent
    correctAnim.start()
  }

  Image { // piano background
    cache: false
    source: Noo.pix("pianoBg")
    width: score.width; height: width * (sourceSize.height / sourceSize.width)
    y: -height
    z: -1
  }

  Component {
    id: blackKeyComp
    Image {
      id: blackKey
      property int nr: index
      source: Noo.pix("pianokey")
      width: Math.round(keyWidth * 0.8); height: instrItem.height / 2
      x: -keyWidth * 0.4; y: octaveCover.height
      z: 3
      MouseArea {
        id: ma
        anchors.fill: parent
        hoverEnabled: !readOnly
        onEntered: activeKey = blackKey
        onClicked: selectedKey = blackKey
      }
    }
  }

  Row {
    Rectangle {
      width: (instrItem.width - keysNumber * Math.floor(keyWidth)) / 2
      height: instrItem.height
      color: "black"
    }
    Repeater {
      id: whiteRep
      model: keysNumber
      Rectangle {
        id: key
        property int nr: index
        property Item black: null
        width: keyWidth; height: instrItem.height; radius: width / 5; color: "white"
        border { width: Math.round(keyWidth / 16); color: "black" }
        MouseArea {
          id: ma
          anchors.fill: parent
          hoverEnabled: !readOnly
          onEntered: activeKey = key
          onClicked: selectedKey = key
        }
        Component.onCompleted: {
          if (index % 7 !== 0 && index % 7 !== 3)
            black = blackKeyComp.createObject(key, { "nr": index })
        }
      }
    }
    Rectangle {
      width: (instrItem.width - keysNumber * Math.floor(keyWidth)) / 2
      height: instrItem.height
      color: "black"
    }
  }

  keyHighlight: Rectangle { // piano key highlight
    parent: selectedKey
    anchors.fill: parent ? parent : undefined
    color: GLOB.selectedColor
    border { width: Math.round(keyWidth / 16); color: "black" }
    radius: width / 5
    z: 2
  }

  Rectangle { // piano key cursor
    anchors.fill: parent ? parent : undefined
    parent: activeKey
    z: 2
    color: GLOB.fingerColor
    border { width: Math.round(keyWidth / 16); color: "black" }
    radius: width / 5
    visible: active
  }

  Rectangle {
    id: octaveCover
    width: instrItem.width; height: Noo.fontSize() * (Noo.isAndroid() ? 1 : 1.5)
    color: "black"
    Repeater {
      model: Math.floor(keysNumber / 7)
      Rectangle {
        x: margin + index * width
        width: keyWidth * 7; height: parent.height
        color: index % 2 ? Qt.rgba(1, 1, 1, 0.2) : "black"
        Text {
          anchors.centerIn: parent
          text: octaveName(firstOctave + index) + (GLOB.scientificOctaves ? "  [%1]".arg(firstOctave + index + 3) : "")
          font { pixelSize: parent.height * 0.8 }
          color: "white"
        }
      }
    }
  }
}
