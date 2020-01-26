/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
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

  OutScaleTip { visible: !active && outOfScale }

  onClearGuitar: { if (fretView) fretView.scale = 0 }
  onEnabledChanged: { if (fretView && !enabled) fretView.scale = 0 }

  property var fretView: null
  MouseArea {
    anchors.fill: parent
    onClicked: {
      if (Noo.fingerPixels() * 4 > height * 1.1) {
          if (!fretView) {
            var fv = Qt.createComponent("qrc:/instruments/FretView.qml")
            fretView = fv.createObject(nootkaWindow.contentItem.parent)
          }
          fretView.scale = 1
          fretView.x = Noo.bound(0, mouse.x, width - fretView.width)
      } else
          pressedAt(mouse.x, mouse.y)
    }
  }
}
