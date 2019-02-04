/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9


SequentialAnimation {
  property Item noteHead: null
  property real endY: 0

  signal applyCorrect()

  // private
  property real halfY: 0

  onRunningChanged: halfY = noteHead.y + (noteHead.y - endY) / 2

  SequentialAnimation { // cross blinking when no note was selected
    loops: noteHead && noteHead.parent.notePosY === 0 ? 2 : 0
    NumberAnimation {
      property: "disp"; from: 0; to: 200; duration: 200
      target: Rectangle {
        id: cross
        parent: noteHead ? noteHead.parent : null; visible: disp > 100; rotation: 45; z: 10
        property int disp: 0
        width: parent ? parent.width * 2 : 0; height: width / 15; color: GLOB.wrongColor; anchors.centerIn: parent; radius: width / 4
        Rectangle { height: parent.width; width: parent.height; color: GLOB.wrongColor; anchors.centerIn: parent; radius: height / 4 }
      }
    }
    NumberAnimation { target: cross; property: "disp"; from: 200; to: 0; duration: 200 }
  }

  ScriptAction { script: noteHead.visible = true }

  ParallelAnimation {
    NumberAnimation { target: noteHead; property: "y"; to: halfY; duration: 300 }
    NumberAnimation { target: noteHead; property: "scale"; to: 3; duration: 300 }
  }

  ScriptAction { script: applyCorrect() }

  ParallelAnimation {
    NumberAnimation { target: noteHead; property: "y"; to: endY; duration: 300 }
    NumberAnimation { target: noteHead; property: "scale"; to: 1; duration: 300 }
  }
}
