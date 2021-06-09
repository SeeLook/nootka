/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka 1.0


TguitarBg {
  id: instrItem

  // private
  property var correctAnim: null
  property var guitarZoom: null

  width: Math.max(nootkaWindow.width, GLOB.instrument.getItemHeight(nootkaWindow.height) * 5.4)
  height: GLOB.instrument.getItemHeight(nootkaWindow.height)

  transformOrigin: Item.TopLeft
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
    if (NOO.isAndroid() && NOO.fingerPixels() * 4 > height)
      guitarZoom = Qt.createComponent("qrc:/instruments/InstrumentZoom.qml").createObject(instrItem)
  }

  MouseArea {
    property point startPos: Qt.point(0, 0)
    enabled: (NOO.fingerPixels() * 4 <= height) || (guitarZoom && instrItem.scale > 1)
    width: parent.width; height: parent.height
    onPressed: startPos = Qt.point(mouseX, mouseY)
    onReleased: {
      var dx = mouseX - startPos.x
      var dy = mouseY - startPos.y
      if (Math.sqrt(dx * dx + dy * dy) < fretWidth / 2)
        pressedAt(mouse.x, mouse.y)
    }
  }

  OutScaleTip { show: !active && outOfScale }
}
