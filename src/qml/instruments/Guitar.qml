/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka 1.0


TguitarBg {
  id: instrItem

  // private
  property var bodyPix: ["", "-electro", "-bass"]
  property var correctAnim: null

  anchors.fill: parent

  onCorrectInstrument: {
    if (!correctAnim) {
      var c = Qt.createComponent("qrc:/exam/CorrectInstrAnim.qml")
      correctAnim = c.createObject(instrItem)
    }
    correctAnim.doCross = wrongItem === null
    correctAnim.start()
  }

  Image { // body
    cache: false
    source: GLOB.instrument.isGuitar ? Noo.pix("body" + bodyPix[GLOB.instrument.typeINT - 1]) : ""
    height: parent.height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 4 : 3.1)
    width: height * (sourceSize.width / sourceSize.height)
    x: GLOB.instrument.type === Tinstrument.ClassicalGuitar ? xiiFret : parent.width * 0.65
    y: parent.height - height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 0.95 : 0.97)
    z: -1
  }

  Image { // rosette/pickup
    cache: false
    source: GLOB.instrument.isGuitar ? Noo.pix(GLOB.instrument.type === Tinstrument.ClassicalGuitar ? "rosette" : "pickup") : ""
    height: parent.height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 1.55 : 1.3)
    width: height * (sourceSize.width / sourceSize.height)
    x: GLOB.instrument.type === Tinstrument.ClassicalGuitar ? fbRect.width - height * 0.25 : parent.width * 0.87
    y: parent.height - height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ?  0.95 : 0.88)
    z: -1
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
    radius: Noo.fontSize() / 3
    source: finger
    opacity: active && fingerPos.x > 0 ? 1 : 0
    Behavior on opacity { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
  }

  Rectangle {
    z: 5
    color: finger.color
    width: parent.width - stringsGap - 2
    height: string < 6 ? strWidth(string) * 1.5 : 0
    x: 1
    y: string < 6 ? fbRect.y + stringsGap / 2 + string * stringsGap - height / 3 : 0
    visible: active && string < 6 && fingerPos.x == 0
  }

  OutScaleTip { visible: !active && outOfScale }
}
