/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7

import Score 1.0

/**
 * Control displaying prompt for adding note or note cursor when hovered
 */
Item {
  id: noteAdd

  signal add()

  property TnoteItem lastNote: null
  property bool active: false
  property alias noteText: note.text
  property alias alterText: cursor.alterText
  property alias yPos: cursor.yPos

  readonly property real alterWidth: 0 // dummy, NoteCursor looks for it

  parent: score.lastStaff
  height: parent.height
  width: 4
  x: lastNote ? lastNote.rightX + 1.5 : score.firstStaff.firstNoteX
  z: 11

  Behavior on x { enabled: GLOB.useAnimations; SpringAnimation { spring: 2; damping: 0.3; duration: 300 }}

  Text {
    id: note
    visible: !active
    font { family: "nootka"; pixelSize: 8 }
    color: Qt.rgba(activPal.highlight.r, activPal.highlight.g, activPal.highlight.b, 0.8)
    x: (4 - width) / 2
    y: 15

    Text {
      y: 1.5
      color: activPal.highlightedText
      font { family: "nootka"; pixelSize: 15 }
      text: "+"
      x: (parent.width - width) / 2
      z: -1
    }
  }

  NoteCursor { id: cursor }

  MouseArea {
    hoverEnabled: true
    anchors.fill: parent
    onClicked: noteAdd.add()
    onEntered: active = true
    onExited: { cursor.yPos = 0; active = false }
    onMouseYChanged: {
      if (score.clef === Tclef.PianoStaffClefs && mouseY >= score.upperLine + 10.4 && mouseY <= score.upperLine + 12.6)
        return
      cursor.yPos = Math.floor(mouseY)
    }
  }
}
