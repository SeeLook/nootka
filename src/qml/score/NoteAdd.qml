/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12

import Nootka 1.0
import Score 1.0


/**
 * Control displaying prompt for adding note or note cursor when hovered/pressed
 */
TaddNoteItem {
  id: noteAdd

  property alias hiTimer: hiTimer

  scoreObject: score.scoreObj
  parent: score.lastStaff
  height: parent ? parent.height : 0
  width: 4
  x: scoreObj.lastNote && !score.readOnly ? scoreObj.lastNote.rightX + 1.5 : score.firstStaff.firstNoteX
  z: 11
  visible: scoreObj.allowAdding && scoreObj.editMode

  Behavior on x { enabled: GLOB.useAnimations; SpringAnimation { spring: 3; damping: 0.2; duration: 300 }}

  // private
  property bool blink: true

  Rectangle { // highlight
    visible: currentNote === null && !cursor.visible && (blink || !hiTimer.running)
    width: 6; height: 12; x: -1; y: 14
    color: NOO.alpha(activPal.highlight, 75)
    radius: 2
  }

  Timer {
    id: hiTimer
    interval: 500; repeat: true
    running: SOUND.listening && !SOUND.tunerMode
    onTriggered: blink = !blink
  }

  Text {
    id: note
    visible: !active && !score.readOnly
    font { family: "nootka"; pixelSize: 8 }
    color: NOO.alpha(GLOB.noteCursorColor, 200)
    x: (4 - width) / 2
    y: 15
    text: NOO.rhythmText(scoreObj.workRhythm)
  }

  NoteCursor { id: cursor; yPos: noteAdd.yPos; headText: scoreObj.workRtmText }

}
