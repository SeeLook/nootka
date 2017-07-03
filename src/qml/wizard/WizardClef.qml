/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0
import "../"


Flickable {
  clip: true
  contentHeight: clefCol.height
  contentWidth: width

  Item {
    id: clefCol
    width: parent.width; height: childrenRect.height

    Loader { sourceComponent: nootkaWindow.instrument === 0 ? noInstrComp : null }
    Component {
      id: noInstrComp
      Column {
        parent: clefCol
        width: parent ? parent.width : 0
        spacing: nootkaWindow.fontSize
        Text {
          width: parent.width
          font { pixelSize: nootkaWindow.fontSize * 2; bold: true }
          wrapMode: Text.WordWrap
          horizontalAlignment: Text.AlignHCenter
          color: activPal.text
          text: qsTr("Select a clef and scale of notes appropriate for your instrument.")
        }
        Score {
          height: nootkaWindow.fontSize * 20
          width: nootkaWindow.fontSize * 12
          anchors.horizontalCenter: parent.horizontalCenter
          meter: Tmeter.NoMeter
          scoreObj.showNoteNames: true
          scoreObj.nameColor: GLOB.nameColor
          scoreObj.nameStyle: GLOB.noteNameStyle
          Component.onCompleted: {
            addNote(scoreObj.lowestNote())
            addNote(scoreObj.highestNote())
          }
          onClefChanged: {
//             sc.addNote(scoreObj.lowestNote())
//             sc.addNote(scoreObj.highestNote())
          }
        }
      }
    }

// Classical or electric guitar
    Loader { sourceComponent: nootkaWindow.instrument === 1 || nootkaWindow.instrument === 2 ? classicComp : null }
    Component {
      id: classicComp
      Column {
        parent: clefCol
        spacing: nootkaWindow.fontSize
        width: parent ? parent.width : 0
        Text {
          width: parent.width
          wrapMode: Text.WordWrap
          font { pixelSize: nootkaWindow.fontSize * 1.2; bold: true }
          horizontalAlignment: Text.AlignHCenter
          color: activPal.text
          text: qsTr("Guitar notation uses the treble clef with the digit \"eight\" written below <br>(even if some editors are forgetting about this digit).<br>Try to understand this.")
        }
        Row {
          anchors.horizontalCenter: parent.horizontalCenter
          spacing: nootkaWindow.fontSize
          Repeater {
            model: 2
            Score {
              id: sc
              height: nootkaWindow.fontSize * 20
              width: nootkaWindow.fontSize * 9
              meter: Tmeter.NoMeter
              readOnly: true
              clef: index === 1 ? Tclef.Treble_G_8down : Tclef.Treble_G
              scoreObj.clefType: clef
              scoreObj.showNoteNames: true
              scoreObj.nameColor: GLOB.nameColor
              scoreObj.nameStyle: GLOB.noteNameStyle
              Component.onCompleted: addNote(Noo.note(1, 1, 0, Trhythm.NoRhythm))
            }
          }
        }
        Text {
          width: parent.width
          wrapMode: Text.WordWrap
          horizontalAlignment: Text.AlignHCenter
          color: activPal.text
          text: "<b><font size=\"5\">Both pictures above show the same note!</font></b><br>(note c in one-line octave)"
        }
      }
    }

  }
}
