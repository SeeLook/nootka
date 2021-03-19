/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka 1.0


TguitarBg {
  id: instrItem

  // private
  property var bodyPix: ["", "-electro", "-bass"]
  property var correctAnim: null
  property var guitarZoom: null

  width: Math.max(nootkaWindow.width, GLOB.instrument.getItemHeight(nootkaWindow.height) * 5.4)
  height: GLOB.instrument.getItemHeight(nootkaWindow.height)

  transformOrigin: Item.BottomLeft
  Behavior on scale {
    enabled: GLOB.useAnimations
    NumberAnimation {
      duration: 150
      onRunningChanged:  {
        if (guitarZoom && !running && scale > 1)
          instrFlick.contentX = guitarZoom.flickX
      }
    }
  }

  onCorrectInstrument: {
    if (!correctAnim)
      correctAnim = Qt.createComponent("qrc:/exam/CorrectInstrAnim.qml").createObject(instrItem)
    correctAnim.doCross = wrongItem === null
    correctAnim.start()
  }

  Item {
    parent: nootkaWindow.contentItem
    width: instrFlick.width; height: instrFlick.height; y: instrFlick.y
    x: -instrFlick.contentX
    transformOrigin: Item.BottomLeft
    scale: instrItem.scale
    Image { // body
      cache: false
      source: GLOB.instrument.isGuitar ? NOO.pix("body" + bodyPix[GLOB.instrument.typeINT - 1]) : ""
      height: parent.height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 4 : 3.1)
      width: height * (sourceSize.width / sourceSize.height)
      x: GLOB.instrument.type === Tinstrument.ClassicalGuitar ? xiiFret : parent.width * 0.65
      y: parent.height - height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 0.95 : 0.97)
    }
    Image { // rosette/pickup
      cache: false
      source: GLOB.instrument.isGuitar ? NOO.pix(GLOB.instrument.type === Tinstrument.ClassicalGuitar ? "rosette" : "pickup") : ""
      height: parent.height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 1.55 : 1.3)
      width: height * (sourceSize.width / sourceSize.height)
      x: GLOB.instrument.type === Tinstrument.ClassicalGuitar ? fbRect.width - height * 0.25 : parent.width * 0.87
      y: parent.height - height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ?  0.95 : 0.88)
    }
  }

  Rectangle {
    id: finger
    color: GLOB.fingerColor
    width: fretWidth / 1.6
    height: width * 0.65
    radius: width * 0.5
    x: fingerPos.x
    y: fingerPos.y
    visible: false
  }

  DropShadow {
    id: fingerShadow
    z: 5
    anchors.fill: finger
    horizontalOffset: finger.height / 6
    verticalOffset: finger.height / 6
    color: "black"
    radius: NOO.factor() / 3
    source: finger
    visible: fingerPos.x > 0
    scale: !pressed && active && fingerPos.x > 0 ? 1 : 0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  }

  Rectangle {
    z: 5
    color: finger.color
    width: parent.width - stringsGap - 2
    height: string < 6 ? strWidth(string) * 1.5 : 0
    x: 1
    y: string < 6 ? fbRect.y + stringsGap / 2 + string * stringsGap - height / 3 : 0
    visible: !pressed && active && string < 6 && fingerPos.x == 0
  }

  Component.onCompleted: {
    if (NOO.isAndroid() && NOO.fingerPixels() * 4 > height * 1.1)
      guitarZoom = Qt.createComponent("qrc:/instruments/InstrumentZoom.qml").createObject(instrItem)
  }

  MouseArea {
    id: touchArea
    enabled: guitarZoom && instrItem.scale > 1
    anchors.fill: parent
    onClicked: pressedAt(mouse.x, mouse.y)
  }

  OutScaleTip { show: !active && outOfScale }
}
