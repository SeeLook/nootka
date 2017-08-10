/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka 1.0
import "instruments"


TguitarBg {

  // private
  property var bodyPix: ["", "-electro", "-bass"]

  anchors.fill: parent

  Image { // body
    cache: false
    source: GLOB.instrument.type === Tinstrument.Piano ? "" : Noo.pix("body" + bodyPix[GLOB.instrument.typeINT - 1])
    height: parent.height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 4.0 : 3.1)
    width: height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 1.452482269503546 : 1.04885993485342)
    x: GLOB.instrument.type === Tinstrument.ClassicalGuitar ? xiiFret : parent.width * 0.65
    y: parent.height - height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 0.95 : 0.97)
    z: -1
  }

  Image { // rosette/pickup
    cache: false
    source: Noo.pix(GLOB.instrument.type === Tinstrument.ClassicalGuitar ? "rosette" : "pickup")
    height: parent.height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 1.5 : 1.3)
    width: height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 1.215351812366738 : 0.6217948717948718)
    x: GLOB.instrument.type === Tinstrument.ClassicalGuitar ? fbRect.width - height * 0.2 : parent.width * 0.87
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
    y: fingerPos.y - height * 0.15
    visible: false
  }

  DropShadow {
    id: fingerShadow
    anchors.fill: finger
    horizontalOffset: stringsGap / 7
    verticalOffset: stringsGap / 7
    color: activPal.shadow
    radius: 8.0
    source: finger
    visible: active && fingerPos.x > 0
  }

  Rectangle {
    color: finger.color
    width: parent.width - stringsGap - 2
    height: string < 6 ? strWidth(string) * 1.5 : 0
    x: 1
    y: string < 6 ? fbRect.y + stringsGap / 2 + string * stringsGap - height / 3 : 0
    visible: active && string < 6 && fingerPos.x == 0
  }

  OutScaleTip { visible: !active && outOfScale }
}
