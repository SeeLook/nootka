/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


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
    NumberAnimation { target: textItem; property: "x"; to: textItem.x + textItem.height / 4; duration: 50 }
    NumberAnimation { target: textItem; property: "x"; to: textItem.x - textItem.height / 2; duration: 100 }
    NumberAnimation { target: textItem; property: "x"; to: textItem.x + textItem.height / 4; duration: 50 }
  }
  PauseAnimation { duration: 100 }
  ParallelAnimation {
    NumberAnimation { target: textItem; property: "opacity"; to: 0; duration: doCross ? 1 : 200 }
    NumberAnimation { target: textItem; property: "scale"; to: 0.1; duration: doCross ? 1 : 200 }
  }
  ScriptAction { script: noteName.applyCorrect() }
  ParallelAnimation {
    NumberAnimation { target: textItem; property: "opacity"; to: 1; duration: 300 }
    SpringAnimation { target: textItem; property: "scale"; from: 0.1; to: 3; spring: 2; damping: 0.2; epsilon: 0.005; duration: 300 }
  }
  ScriptAction { script: noteName.finishCorrectAnim() }
}
