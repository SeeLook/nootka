/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0
import "../"


Tflickable {
  id: melPage
  height: parent.height
  contentHeight: melodyCol.height + Noo.fontSize() * 2
  contentWidth: width

  Column {
    id: melodyCol
    width: parent.width
    enabled: creator.isMelody

    Tile {
      id: topTile
      property var descList: [ qsTr("Melodies are composed from a note range defined on the 'Range' page."),
                               qsTr("Melodies are composed from notes selected on the score below."),
                               qsTr("Melodies are randomized from defined list of melodies.") ]
      description: descList[melCombo.currentIndex]
      Row {
        spacing: Noo.fontSize()
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
          text: Noo.TR("TmelodySettings", "Random melody")
          anchors.verticalCenter: parent.verticalCenter
          color: enabled ? activPal.text : disdPal.text
        }
        ComboBox {
          id: melCombo
          width: Noo.fontSize() * 15
          model: [ qsTr("in selected range"), qsTr("from notes below"), qsTr("from the list") ]
          currentIndex: creator.randMelody
          onActivated: creator.randMelody = currentIndex
        }
      }
    }
    Grid {
      id: melGrid
      spacing: melPage.width / 100
      columns: Noo.fontSize() * 50 > melPage.width ? 1 : 2
      anchors.horizontalCenter: parent.horizontalCenter
      visible: melCombo.currentIndex !== 2
      Tile {
        description: Noo.TR("TmelodySettings", "Maximum number of notes in a melody. Melody length is random value between 70% and 100% of that number.")
        anchors.horizontalCenter: undefined
        width: Math.max(lenRow.width + Noo.fontSize() * 4, melPage.width * 0.49)
        Row {
          id: lenRow
          spacing: Noo.fontSize()
          anchors.horizontalCenter: parent.horizontalCenter
          Text {
            text: Noo.TR("TmelodySettings", "Melody length")
            anchors.verticalCenter: parent.verticalCenter
            color: enabled ? activPal.text : disdPal.text
          }
          SpinBox {
            id: lenSpin
            editable: true
            from: 1; to: 50
            value: creator.melodyLen
          }
          Connections {
            target: lenSpin
            onValueModified: creator.melodyLen = lenSpin.value
          }
        }
      }
      Tile {
        anchors.horizontalCenter: undefined
        width: Math.max(tonicChB.width + Noo.fontSize() * 4, melPage.width * 0.49)
        description: qsTr("Determines the last note of a melody.<br>When set, melody will be finished on tonic note in actual key signature.")
        TcheckBox {
          enabled: melCombo.currentIndex === 0
          id: tonicChB
          anchors.horizontalCenter: parent.horizontalCenter
          text: qsTr("Melody ends on tonic note")
          checked: creator.endsOnTonic
          onClicked: creator.endsOnTonic = checked
        }
      }
    }
    Score {
      id: listScore
      visible: melCombo.currentIndex === 1
      width: parent.width; height: melPage.height - topTile.height - melGrid.height
      meter: Tmeter.NoMeter
      enableKeySign: creator.useKeySign
      Connections {
        target: creator
        onUpdateNotesList: updateNotesList()
      }
    }
  }
  Component.onCompleted: {
    if (listScore.visible)
      updateNotesList()
  }
  function updateNotesList() {
    listScore.clearScore()
    listScore.clef = creator.clef
    listScore.scoreObj.clefType = creator.clef
    listScore.setKeySignature(creator.keyOfRandList)
    for (var i = 0; i < creator.notesInList(); ++i)
      listScore.addNote(creator.noteFromList(i))
  }
  function saveLevel() {
    if (melCombo.currentIndex === 1) {
      creator.keyOfRandList = listScore.scoreObj.keySignature
      for (var i = 0; i < listScore.notesCount; ++i)
        creator.setNoteOfList(i, listScore.scoreObj.noteAt(i))
    }
  }
}
