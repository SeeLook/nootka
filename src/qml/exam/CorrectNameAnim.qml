/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12


SequentialAnimation {
  property bool doCross: false
  SequentialAnimation { // cross blinking
    loops: doCross ? 2 : 0
    NumberAnimation {
      property: "disp"; from: 0; to: 200; duration: 200
      target: Rectangle {
        id: cross; parent: textItem.parent; visible: disp > 100; rotation: 45; radius: height / 2
        property int disp: 0
        width: parent.height * 1.4; height: parent.height / 15; color: GLOB.wrongColor; anchors.centerIn: parent
        Rectangle { height: parent.width; width: parent.height; color: GLOB.wrongColor; anchors.centerIn: parent; radius: width }
      }
    }
    NumberAnimation { target: cross; property: "disp"; from: 200; to: 0; duration: 200 }
  }
  SequentialAnimation { // shake text
    loops: doCross ? 0 : 2
    NumberAnimation { target: textItem.anchors; property: "horizontalCenterOffset"; to: textItem.height / 4; duration: 50 }
    NumberAnimation { target: textItem.anchors; property: "horizontalCenterOffset"; to: -textItem.height / 4; duration: 100 }
    NumberAnimation { target: textItem.anchors; property: "horizontalCenterOffset"; to: 0; duration: 50 }
  }
  PauseAnimation { duration: 100 }
  ParallelAnimation {
    NumberAnimation { target: textItem; property: "opacity"; to: 0; duration: doCross ? 1 : 200 }
    NumberAnimation { target: textItem; property: "scale"; to: 2; duration: doCross ? 1 : 200 }
  }
  ScriptAction { script: noteName.applyCorrect() }
  ParallelAnimation {
    NumberAnimation { target: textItem; property: "opacity"; to: 1; duration: 150 }
    NumberAnimation { target: textItem; property: "scale"; to: 6; duration: 200 }
  }
  NumberAnimation { target: textItem; property: "scale"; to: 3; duration: 200 }
  ScriptAction { script: noteName.finishCorrectAnim() }
}
