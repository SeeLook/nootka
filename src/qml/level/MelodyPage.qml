/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0
import "../"
import "../score"


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
          text: qsTr("Random melody")
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
      spacing: melPage.width / 50
      columns: Noo.fontSize() * 50 > melPage.width ? 1 : (creator.hasRhythms ? 1 : 2)
      anchors.horizontalCenter: parent.horizontalCenter
      visible: melCombo.currentIndex !== 2
      Tile {
        visible: !creator.hasRhythms
        description: qsTr("Maximum number of notes in a melody. Melody length is random value between 70% and 100% of that number.")
        anchors.horizontalCenter: undefined
        width: Math.max(lenRow.width + Noo.fontSize() * 4, melPage.width * 0.49)
        Row {
          id: lenRow
          spacing: Noo.fontSize()
          anchors.horizontalCenter: parent.horizontalCenter
          Text {
            text: qsTr("Melody length")
            anchors.verticalCenter: parent.verticalCenter
            color: enabled ? activPal.text : disdPal.text
          }
          SpinBox {
            id: lenSpin
            editable: true
            from: 1; to: 50
            value: creator.melodyLen
            onValueModified: creator.melodyLen = lenSpin.value
          }
        }
      }
      EndOnTonicTile {
        width: Math.max(checkBox.width + Noo.fontSize() * 4, melPage.width * 0.49)
        checked: creator.endsOnTonic
        checkBox.onClicked: creator.endsOnTonic = checked
      }
    }
    Score {
      id: listScore
      visible: melCombo.currentIndex === 1
      width: parent.width; height: melPage.height - topTile.height - melGrid.height - tempoRange.height
      meter: Tmeter.NoMeter
      enableKeySign: creator.useKeySign
      scoreObj.allowAdding: visible
    }
    Tile {
      Row {
        spacing: Noo.fontSize()
        anchors.horizontalCenter: parent.horizontalCenter
        TcheckBox {
            id: inTempoChB
            text: qsTr("Play in tempo")
  //           checked: creator.endsOnTonic
  //           onClicked: creator.endsOnTonic = checked
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          text: Math.floor(tempoRange.first.value); font { bold: true; pixelSize: Noo.fontSize() * 0.8 }
          color: enabled ? activPal.text : disdPal.text
        }
        RangeSlider {
          id: tempoRange
          width: parent.parent.width / 3
          first.value: 60; second.value: 120
          from: 40; to: 180
          stepSize: 5; snapMode: RangeSlider.SnapAlways
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          text: Math.floor(tempoRange.second.value); font { bold: true; pixelSize: Noo.fontSize() * 0.8 }
          color: enabled ? activPal.text : disdPal.text
        }
      }
    }
  }

  Component.onCompleted: {
    if (listScore.visible)
      updateNotesList()
    checkMelodyView()
  }

  Connections {
    target: creator
    onUpdateNotesList: updateNotesList()
    onUpdateMelodyList: checkMelodyView()
  }

  property var melListView: null
  Connections {
    target: melCombo
    onCurrentIndexChanged: checkMelodyView()
  }

  function checkMelodyView() {
    if (melCombo.currentIndex === 2) {
        if (!melListView) {
          var c = Qt.createComponent("qrc:/level/MelodyListView.qml")
          melListView = c.createObject(melodyCol)
          melListView.width = Qt.binding(function() { return melodyCol.width - 10 })
          melListView.height = Qt.binding(function() { return melPage.height - topTile.height })
        }
        melListView.setLevel(creator.level())
    } else if (melListView) {
        melListView.destroy()
    }
  }

  function updateNotesList() {
    listScore.clearScore()
    listScore.clef = creator.clef
    listScore.keySignature = creator.keyOfRandList
    for (var i = 0; i < creator.notesInList(); ++i)
      listScore.addNote(creator.noteFromList(i))
  }

  function saveLevel() {
    if (melCombo.currentIndex === 1) { // melody from note list
        creator.keyOfRandList = listScore.scoreObj.keySignature
        for (var i = 0; i < listScore.notesCount; ++i)
          creator.setNoteOfList(i, listScore.scoreObj.noteAt(i))
    } else if (melCombo.currentIndex === 2) { // random melody from set
        melListView.save()
    }
  }
}
