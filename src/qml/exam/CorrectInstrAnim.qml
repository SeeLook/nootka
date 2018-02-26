/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


SequentialAnimation {
  property bool doCross: false
  property var wItem: instrItem.wrongItem
  property real shakeStep: wItem ? Math.min(wItem.height, instrItem.height / 10) : instrItem.height / 10
  SequentialAnimation { // cross blinking
    loops: doCross ? 2 : 0
    NumberAnimation {
      property: "disp"; from: 0; to: 200; duration: 200
      target: Rectangle {
        id: cross; parent: instrItem; visible: disp > 100; rotation: 45; z: 10
        property int disp: 0
        width: parent.height * 1.4; height: parent.height / 20; color: GLOB.wrongColor; anchors.centerIn: parent
        Rectangle { height: parent.width; width: parent.height; color: GLOB.wrongColor; anchors.centerIn: parent }
      }
    }
    NumberAnimation { target: cross; property: "disp"; from: 200; to: 0; duration: 200 }
  }
  SequentialAnimation { // shake
    loops: doCross ? 0 : 2
    NumberAnimation { target: wItem; property: "y"; to: wItem ? wItem.y + shakeStep / 2 : 0; duration: 50 }
    NumberAnimation { target: wItem; property: "y"; to: wItem ? wItem.y - shakeStep : 0; duration: 100 }
    NumberAnimation { target: wItem; property: "y"; to: wItem ? wItem.y + shakeStep / 2 : 0; duration: 50 }
  }
  PauseAnimation { duration: 50 }
  ParallelAnimation {
    loops: wItem ? 1 : 0
    NumberAnimation { target: wItem; property: "opacity"; to: 0; duration: doCross ? 1 : 200 }
    NumberAnimation { target: wItem; property: "scale"; to: 0.1; duration: doCross ? 1 : 200 }
  }
  ScriptAction { script: instrItem.applyCorrect() }
  ParallelAnimation {
    NumberAnimation { target: instrItem.goodItem; property: "opacity"; from: 0; to: 1; duration: 200 } // 5 is bandoneon
    NumberAnimation { target: instrItem.goodItem; property: "scale"; from: 0.1; to: GLOB.instrument.type === 5 ? 1.2 : 1; duration: 200 }
  }
  SequentialAnimation {
    NumberAnimation { target: instrItem.goodItem; property: "scale"; from: 1; to: GLOB.instrument.type === 5 ? 2.5 : 2; duration: 200 }
    NumberAnimation { target: instrItem.goodItem; property: "scale"; from: 2; to: GLOB.instrument.type === 5 ? 1.2 : 1; duration: 200 }
  }
  ScriptAction { script: instrItem.finishCorrectAnim() }
  onStopped: destroy()
}
