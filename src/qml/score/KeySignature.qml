/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
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

  Loader { sourceComponent: accidsComp; y: 0 }
  Loader { sourceComponent: score.clef === Tclef.PianoStaffClefs ? accidsComp : null; y: 12 }

  Component {
    id: accidsComp
    Repeater {
      model: 7
      Text {
        font { family: "Scorek"; pixelSize: 8 }
        color: activPal.text
        text: score.keySignature < 0 ? "\ue260" : (score.keySignature > 0 ? "\ue262" : "") // flat or sharp symbols
        x: index * 1.8
        y: parent.y + (score.keySignature < 0 ? flatPos[index] : sharpPos[index]) - accidOffset(score.clef)
        + (score.clef === Tclef.Tenor_C && score.keySignature > 0 && (index === 0 || index === 2) ? 7 : 0)
        opacity: index < Math.abs(score.keySignature) ? 1.0 : 0.0
        Behavior on opacity { enabled: GLOB.useAnimations; NumberAnimation { property: "opacity"; duration: 300 }}
      }
    }
  }

  Loader { sourceComponent: mAreaComp; y: 13 }
  Loader { sourceComponent: score.clef === Tclef.PianoStaffClefs ? mAreaComp : null; y: 34 }

  Component {
    id: mAreaComp
    MouseArea { // area at lower staff
      width: keySig.width; height: 14
      enabled: !scoreObj.readOnly || !scoreObj.keyReadOnly
      hoverEnabled: true
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
      onEntered: Noo.setStatusTip(qsTr("Key signature - to change it, click above or below the staff or use mouse wheel."), Item.TopLeft)
      onExited: Noo.setStatusTip("", Item.TopLeft)
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
