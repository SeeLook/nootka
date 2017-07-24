/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Score 1.0

/**
 * Control displaying prompt for adding note or note cursor when hovered/pressed
 */
TaddObject {
  id: noteAdd

  property TnoteItem lastNote: null
  property alias noteText: note.text
  property alias alterText: cursor.alterText

  readonly property real alterWidth: 0 // dummy, NoteCursor looks for it

  scoreObject: score.scoreObj
  parent: score.lastStaff
  height: parent.height
  width: 4
  x: lastNote ? lastNote.rightX + 1.5 : score.firstStaff.firstNoteX
  z: 11

  Behavior on x { enabled: GLOB.useAnimations; SpringAnimation { spring: 3; damping: 0.2; duration: 300 }}

  Text {
    id: note
    visible: !active && !score.readOnly
    font { family: "nootka"; pixelSize: 8 }
    color: Noo.alpha(activPal.highlight, 200)
    x: (4 - width) / 2
    y: 15
  }

  NoteCursor { id: cursor; yPos: noteAdd.yPos; headText: scoreObj.workRtmText }

}
