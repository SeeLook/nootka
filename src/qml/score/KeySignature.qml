/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0

import Score 1.0


Item {
  id: keySig

  property int key: 0
  property bool readOnly: false

// those properties are private
  readonly property var sharpPos: [ 0, 3, -1, 2, 5, 1,  4 ]
  readonly property var flatPos: [ 4, 1, 5, 2, 6, 3, 7 ]
  property real accidOff: 0
  property int clef: Tclef.Treble_G

  width: Math.max(6, (Math.abs(key) + 1) * 1.8)
  height: 10

  signal keySignatureChanged(var k)

  onKeyChanged: keySignatureChanged(key)

//   Rectangle { width: parent.width; height: 14; color: "#aaff0000"; y: 12 }

  function changeClef(c) {
      if (c === Tclef.Treble_G || c === Tclef.Treble_G_8down)
          accidOff = 1
      else if (c === Tclef.Bass_F || c === Tclef.Bass_F_8down)
          accidOff = -1
      else if (c === Tclef.Alto_C)
          accidOff = 0;
      else if (c === Tclef.Tenor_C)
          accidOff = 2
      else
          accidOff = 1
      clef = c
  }

  Repeater {
      model: 7
      Text {
        font { family: "Scorek"; pixelSize: 8 }
        color: activPal.text
        text: key < 0 ? "\ue260" : (key > 0 ? "\ue262" : "") // flat or sharp symbols
        x: index * 1.8
        y: (key < 0 ? flatPos[index] : sharpPos[index]) - (accidOff) + (clef === Tclef.Tenor_C && key > 0 && (index === 0 || index === 2) ? 7 : 0)
        opacity: index < Math.abs(key) ? 1.0 : 0.0
        Behavior on opacity { enabled: GLOB.useAnimations; NumberAnimation { property: "opacity"; duration: 200 }}
      }
  }

  MouseArea { // occupy only selected part of staff height
      width: parent.width; height: 14; y: 12
      enabled: !readOnly
      onClicked: {
        if (mouseY < 7) {
            if (key < 7)
              ++key
        } else {
          if (key > -7)
              --key
        }
      }
      onWheel: {
        if (wheel.angleDelta.y > 0) {
          if (key < 7)
            ++key
        } else if (wheel.angleDelta.y < 0) {
          if (key > -7)
            --key
        }
      }
  }
}
