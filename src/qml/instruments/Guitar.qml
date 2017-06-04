/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtGraphicalEffects 1.0

import Nootka 1.0


Item {
  id: root
  property alias note: guitar.note
  // private
  property var bodyPix: ["", "-electro", "-bass"]

  anchors.fill: parent

  TguitarBg {
    id: guitar
    anchors.fill: parent
    onNoteChanged: {
      selectedFinger.x = fingerPos.x;
      selectedFinger.y = fingerPos.y - selectedFinger.height * 0.15
    }
  }

  Image { // body
    cache: false
    source: GLOB.instrument.type === Tinstrument.Piano ? "" : Tpath.pix("body" + bodyPix[GLOB.instrument.typeINT - 1])
    height: parent.height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 4.0 : 3.1)
    width: height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 1.452482269503546 : 1.04885993485342)
    x: GLOB.instrument.type === Tinstrument.ClassicalGuitar ? guitar.xiiFret : parent.width * 0.65
    y: parent.height - height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 0.95 : 0.97)
    z: -1
  }

  Image { // rosette/pickup
    cache: false
    source: Tpath.pix(GLOB.instrument.type === Tinstrument.ClassicalGuitar ? "rosette" : "pickup")
    height: parent.height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 1.5 : 1.3)
    width: height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ? 1.215351812366738 : 0.6217948717948718)
    x: GLOB.instrument.type === Tinstrument.ClassicalGuitar ? guitar.fbRect.width - height * 0.2 : parent.width * 0.87
    y: parent.height - height * (GLOB.instrument.type === Tinstrument.ClassicalGuitar ?  0.95 : 0.88)
    z: -1
  }

  Rectangle {
    id: selectedFinger
    color: GLOB.selectedColor
    width: guitar.fretWidth / 1.6
    height: width * 0.65
    radius: width * 0.5
    visible: x != 0
  }

  Rectangle {
    id: finger
    color: GLOB.fingerColor
    width: guitar.fretWidth / 1.6
    height: width * 0.65
    radius: width * 0.5
    x: guitar.fingerPos.x
    y: guitar.fingerPos.y - height * 0.15
    visible: false
  }

  DropShadow {
    id: fingerShadow
    anchors.fill: finger
    horizontalOffset: guitar.stringsGap / 7
    verticalOffset: guitar.stringsGap / 7
    color: activPal.shadow
    radius: 8.0
    source: finger
    visible: guitar.active && guitar.fingerPos.x > 0
  }

  Rectangle {
    id: string
    color: finger.color
    width: guitar.width - guitar.stringsGap - 2
    height: guitar.string < 6 ? guitar.strWidth(guitar.string) * 1.5 : 0
    x: 1
    y: guitar.string < 6 ? 10 + guitar.stringsGap / 2 + guitar.string * guitar.stringsGap - height / 3 : 0
    visible: guitar.active && guitar.string < 6 && guitar.fingerPos.x == 0
  }
}
