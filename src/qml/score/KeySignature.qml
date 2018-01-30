/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0


Item {
  id: keySig

// private
  readonly property var sharpPos: [ 0, 3, -1, 2, 5, 1,  4 ]
  readonly property var flatPos: [ 4, 1, 5, 2, 6, 3, 7 ]

  x: staff.clef.x + staff.clef.width + 1
  width: Math.max(6, (Math.abs(score.keySignature) + 1) * 1.8)
  height: 10

  function accidOffset(c) {
      var accidOff = 1
      switch (c) {
        case Tclef.Bass_F:
        case Tclef.Bass_F_8down:
          accidOff = -1; break;
        case Tclef.Tenor_C:
          accidOff = 2; break;
        case Tclef.Alto_C:
          accidOff = 0; break;
        case Tclef.PianoStaffClefs:
          accidOff = 3; break;
      }
      return accidOff;
  }

  Repeater {
      id: accidRep
      model: 7
      Text {
        font { family: "Scorek"; pixelSize: 8 }
        color: activPal.text
        text: score.keySignature < 0 ? "\ue260" : (score.keySignature > 0 ? "\ue262" : "") // flat or sharp symbols
        x: index * 1.8
        y: (score.keySignature < 0 ? flatPos[index] : sharpPos[index]) - accidOffset(score.clef)
                          + (score.clef === Tclef.Tenor_C && score.keySignature > 0 && (index === 0 || index === 2) ? 7 : 0)
        opacity: index < Math.abs(score.keySignature) ? 1.0 : 0.0
        Behavior on opacity { enabled: GLOB.useAnimations; NumberAnimation { property: "opacity"; duration: 300 }}
      }
  }

  Loader { sourceComponent: score.clef === Tclef.PianoStaffClefs ? lowerAccids : null }
  Component {
      id: lowerAccids
      Repeater {
        model: 7
        Text { // accidentals at lower staff
          property var upperAccid: accidRep.itemAt(index)
          font { family: "Scorek"; pixelSize: 8 }
          color: activPal.text
          text: upperAccid ? upperAccid.text : ""
          y: upperAccid ? upperAccid.y + 24 : 0
          x: upperAccid ? upperAccid.x : 0
          opacity: upperAccid ? upperAccid.opacity : 0
        }
      }
  }

  MouseArea { // occupy only selected part of staff height
      width: parent.width; height: 14; y: 13 - (score.clef === Tclef.PianoStaffClefs ? 2 : 0)
      enabled: !scoreObj.readOnly || !scoreObj.keyReadOnly
      onClicked: {
        if (mouseY < 7)
          keyUp()
        else
          keyDown()
      }
      onWheel: {
        if (wheel.angleDelta.y > 0)
          deltaUp()
        else if (wheel.angleDelta.y < 0)
          deltaDown()
      }
  }

  Loader { sourceComponent: score.clef === Tclef.PianoStaffClefs ? lowerArea : null }
  Component {
      id: lowerArea
      MouseArea { // area at lower staff
          parent: keySig
          width: keySig.width; height: 14; y: 34
          enabled: !scoreObj.readOnly || !scoreObj.keyReadOnly
          onClicked: {
            if (mouseY < 7)
              keyUp()
            else
              keyDown()
          }
          onWheel: {
            if (wheel.angleDelta.y > 0)
              deltaUp()
            else if (wheel.angleDelta.y < 0)
              deltaDown()
          }
      }
  }

  // stops switching keys too quick (by wheel on touch pad)
  Timer { id: wheelTimer; interval: 250 }

  function keyUp() { if (score.keySignature < 7) ++score.keySignature }
  function keyDown() { if (score.keySignature > -7) --score.keySignature }

  function deltaUp() {
    if (!wheelTimer.running) {
      keyUp()
      wheelTimer.running = true
    }
  }

  function deltaDown() {
    if (!wheelTimer.running) {
      keyDown()
      wheelTimer.running = true
    }
  }
}
