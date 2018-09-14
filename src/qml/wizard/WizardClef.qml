/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0
import "../"
import "../score"


Flickable {
  property int clef: noLoader.item ? noLoader.item.score.clef : Noo.instr(nootkaWindow.instrument).clef
  property int transposition: saxLoader.item ? saxLoader.item.transpose.outShift : (noLoader.item ? noLoader.item.transpose.outShift : Noo.instr(nootkaWindow.instrument).transposition)

  clip: true
  contentHeight: mainItem.height
  contentWidth: width

  Item {
    id: mainItem
    width: parent.width; height: childrenRect.height

    Loader { id: noLoader; sourceComponent: nootkaWindow.instrument === 0 ? noInstrComp : null }
    Component {
      id: noInstrComp
      Column {
        property alias score: score
        property alias transpose: transpose
        parent: mainItem
        width: parent ? parent.width : 0
        spacing: Noo.fontSize()
        Text {
          width: parent.width
          font { pixelSize: Noo.fontSize() * 2; bold: true }
          wrapMode: Text.WordWrap
          horizontalAlignment: Text.AlignHCenter
          color: activPal.text
          text: qsTr("Select a clef and scale of notes appropriate for your instrument.")
        }
        Item {
          height: Noo.fontSize() * 20; width: Noo.fontSize() * 12
          anchors.horizontalCenter: parent.horizontalCenter
          Score {
            id: score
            anchors.fill: parent
            meter: Tmeter.NoMeter
            scoreObj.editMode: true
            scoreObj.showNoteNames: true
            scoreObj.nameColor: GLOB.nameColor
            scoreObj.nameStyle: GLOB.noteNameStyle
            Component.onCompleted: {
              addNote(scoreObj.lowestNote())
              addNote(scoreObj.highestNote())
            }
            onClefChanged: {
              setNote(scoreObj.note(0), scoreObj.lowestNote())
              setNote(scoreObj.note(1), scoreObj.highestNote())
            }
          }
        }
        Transposition {
          id: transpose
          shift: 0
        }
      }
    }

// Classical or electric guitar
    Loader { sourceComponent: nootkaWindow.instrument === 1 || nootkaWindow.instrument === 2 ? classicComp : null }
    Component {
      id: classicComp
      Column {
        parent: mainItem
        spacing: Noo.fontSize()
        width: parent ? parent.width : 0
        Text {
          width: parent.width
          wrapMode: Text.WordWrap
          font { pixelSize: Noo.fontSize() * 1.2; bold: true }
          horizontalAlignment: Text.AlignHCenter
          color: activPal.text
          text: qsTr("Guitar notation uses the treble clef with the digit \"eight\" written below <br>(even if some editors are forgetting about this digit).<br>Try to understand this.")
        }
        Row {
          anchors.horizontalCenter: parent.horizontalCenter
          spacing: Noo.fontSize()
          Repeater {
            model: 2
            Score {
              id: sc
              height: Noo.fontSize() * 20
              width: Noo.fontSize() * 9
              meter: Tmeter.NoMeter
              readOnly: true
              clef: index === 1 ? Tclef.Treble_G_8down : Tclef.Treble_G
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

  // Saxophones
  Loader { id: saxLoader; sourceComponent: Noo.instr(nootkaWindow.instrument).isSax ? saxComp : null }
    Component {
      id: saxComp
      Column {
        property alias transpose: transpose
        parent: mainItem
        spacing: Noo.fontSize()
        anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
        width: parent ? parent.width * 0.9 : 0
        Text {
          width: parent.width
          font { pixelSize: Noo.fontSize() * 1.5 }
          wrapMode: Text.WordWrap
          horizontalAlignment: Text.AlignHCenter
          color: activPal.text
          text: qsTr("Saxophones are transposing instruments.<br>It means that note pitch in the score doesn't correspond directly to note which is played - it transposes by interval (number of semitones) according to kind of saxophone.")
        }
        Transposition {
          id: transpose
          anchors.horizontalCenter: parent.horizontalCenter
          shift: Noo.instr(nootkaWindow.instrument).transposition
        }
        Connections {
          target: nootkaWindow
          onInstrumentChanged: transpose.shift = Noo.instr(nootkaWindow.instrument).transposition
        }
        Text {
          anchors.horizontalCenter: parent.horizontalCenter
          font { pixelSize: Noo.fontSize() * 2; bold: true }
          text: Noo.noteName(Noo.note(13 + transpose.outShift, false), 0, false)
        }
      }
    }

  }
}
