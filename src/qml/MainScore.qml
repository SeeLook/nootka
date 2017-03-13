/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
 *   seelook@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0

import Score 1.0


Score {
  id: mainScore

  width: parent.width

  property alias scoreMenu: scoreMenu
  property TnoteItem currentNote: null

  enableKeySign: true

  Rectangle { // note highlight
    parent: currentNote
    visible: currentNote != null
    width: currentNote ? (currentNote.width - currentNote.alterWidth) * 1.5 : 0
    height: currentNote ? Math.min(12.0, currentNote.notePosY + 6.0) : 0
    x: currentNote ? -width * 0.25 : 0
    y: currentNote ? Math.min(currentNote.height - height, Math.max(0.0, currentNote.notePosY - height / 2.0)) : 0
    color: Qt.rgba(activPal.highlight.r, activPal.highlight.g, activPal.highlight.b, 0.3)
    z: -1
    radius: width / 3.0
  }

  NoteCursor {
    parent: scoreObj.activeNote
    anchors.fill: parent
    yPos: scoreObj.activeYpos
    upperLine: scoreObj.upperLine
    onClicked: {
      scoreObj.noteClicked(y)
      mainScore.currentNote = scoreObj.activeNote
    }
  }

  Menu {
      id: scoreMenu
      width: nootkaWindow.fontSize * 15

      MenuItem {
        text: qsTr("Additional accidentals")
        checkable: true
      }
      MenuItem {
        text: qsTr("Show note names")
        checkable: true
      }
      MenuItem {
        id: zoomOut
        text: qsTr("Zoom score out")
        onTriggered: scaleFactor = Math.max(0.4, scaleFactor - 0.2)
      }
      MenuItem {
        id: zoomIn
        text: qsTr("Zoom score in")
        onTriggered:  scaleFactor = Math.min(scaleFactor + 0.2, 1.0)
      }
  }
  Shortcut { sequence: StandardKey.ZoomOut; onActivated: zoomOut.triggered() }
  Shortcut { sequence: StandardKey.ZoomIn; onActivated: zoomIn.triggered() }
  Shortcut {
    sequence: StandardKey.MoveToNextChar;
    onActivated: {
      if (currentNote) {
          if (currentNote.index < notesCount - 1)
            currentNote =  scoreObj.note(currentNote.index + 1)
      } else
          currentNote = scoreObj.note(0)
    }
  }
  Shortcut {
    sequence: StandardKey.MoveToPreviousChar;
    onActivated: {
      if (currentNote) {
          if (currentNote.index > 0)
            currentNote = scoreObj.note(currentNote.index - 1)
      } else {
          currentNote = scoreObj.note(0)
      }
    }
  }
}
