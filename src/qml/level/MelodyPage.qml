/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0
import "../"
import "../score"


Tflickable {
  id: melPage
  height: parent.height
  contentHeight: melodyCol.height + NOO.factor() * 2
  contentWidth: width

  Column {
    id: melodyCol
    width: parent.width
    enabled: creator.isMelody

    Tile {
      id: topTile
      width: parent.width * (NOO.isAndroid() ? 0.98 : 1)
      readonly property string keyRangeText: "<br>" + qsTr("They will be transposed if key signatures are set to be used and any of them differs from the key(s) defined below.")
      property var descList: [ qsTr("Melodies are composed from notes in range defined on the 'Range' page."),
                               qsTr("Melodies are composed from notes selected on the score below."),
                               qsTr("Melodies are selected from the list below.") ]
      description: descList[melCombo.currentIndex] + (melCombo.currentIndex > 0 ? keyRangeText : "")
      Row {
        spacing: NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
          text: NOO.TR("LevelCreator", "Melody")
          anchors.verticalCenter: parent.verticalCenter
          color: enabled ? activPal.text : disdPal.text
        }
        TcomboBox {
          id: melCombo
          width: NOO.factor() * 20
          model: [ qsTr("from notes in range"), qsTr("from selected notes"), qsTr("from set of melodies") ]
          currentIndex: creator.howGetMelody
          onActivated: creator.howGetMelody = currentIndex
        }
      }
    }
    Grid {
      id: melGrid
      spacing: melPage.width / (NOO.isAndroid() ? 100 : 50)
      columns: NOO.factor() * 50 > melPage.width ? 1 : 2 // (melLenTile.visible ? 2 : 1)
      anchors.horizontalCenter: parent.horizontalCenter

      Tile {
        id: melLenTile
        visible: creator.isMelody && !creator.hasRhythms && melCombo.currentIndex !== 2
        description: qsTr("Maximum number of notes in a melody. Melody length is random value between 70% and 100% of that number.")
        anchors.horizontalCenter: undefined
        width: melGrid.columns === 1 ? melPage.width * 0.98 : Math.max(lenRow.width + NOO.factor() * 4, melPage.width * 0.48)
        Row {
          id: lenRow
          spacing: NOO.factor()
          anchors.horizontalCenter: parent.horizontalCenter
          Text {
            text: qsTr("Melody length")
            anchors.verticalCenter: parent.verticalCenter
            color: enabled ? activPal.text : disdPal.text
          }
          TspinBox {
            id: lenSpin
            from: 1; to: 50
            value: creator.melodyLen
            onValueModified: creator.melodyLen = lenSpin.value
          }
        }
      }
      EndOnTonicTile {
        visible: melCombo.currentIndex !== 2
        width: melGrid.columns === 1 ? melPage.width * 0.98 : Math.max(checkBox.width + NOO.factor() * 4, melPage.width * 0.48)
        checked: creator.endsOnTonic
        checkBox.onClicked: creator.endsOnTonic = checked
      }

      Tile {
        width: melGrid.columns === 1 ? melPage.width * 0.98 : Math.max(randOrderChB.width + NOO.factor() * 4, melPage.width * 0.48)
        visible: melCombo.currentIndex === 2
        anchors.horizontalCenter: undefined
        TcheckBox {
          id: randOrderChB
          anchors.horizontalCenter: parent.horizontalCenter
          text: qsTr("random order")
          checked: creator.randomOrder
          onClicked: creator.randomOrder = checked
        }
        description: qsTr("When checked, melodies from the list will be asked in random order.")
      }
      Tile {
        width: melGrid.columns === 1 ? melPage.width * 0.98 : Math.max(repeatRow.width + NOO.factor() * 4, melPage.width * 0.48)
        visible: melCombo.currentIndex === 2
        anchors.horizontalCenter: undefined
        description: qsTr("How many times during an exam a melody from the list has to be played or written correctly. Warning! It multiplies question number.")
        Row {
          id: repeatRow
          spacing: NOO.factor()
          anchors.horizontalCenter: parent.horizontalCenter
          Text {
            text: qsTr("number of repeats")
            anchors.verticalCenter: parent.verticalCenter
            color: enabled ? activPal.text : disdPal.text
          }
          TspinBox {
            id: repeatSpin
            from: 1; to: 15
            value: creator.repeatsNumber
            onValueModified: creator.repeatsNumber = repeatSpin.value
          }
        }
      }
    }
    Item {
      width: parent.width; height: melPage.height - topTile.height - melGrid.height //- tempoRange.height
      visible: melCombo.currentIndex === 1
      Score {
        id: listScore
        visible: melCombo.currentIndex === 1
        anchors.fill: parent
        meter: Tmeter.NoMeter
        enableKeySign: creator.useKeySign
        scoreObj.allowAdding: visible
        scoreObj.editMode: true
      }
    }
    Tile {
      visible: false // TODO
      Row {
        spacing: NOO.factor()
        anchors.horizontalCenter: parent.horizontalCenter
        TcheckBox {
            id: inTempoChB
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Play in tempo")
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          text: Math.floor(tempoRange.first.value); font { bold: true; pixelSize: NOO.factor() * 0.8 }
          color: enabled ? activPal.text : disdPal.text
        }
        RangeSlider { // TODO so far it is only one RangeSlider in Nootka - set its style here according to Tslider
          id: tempoRange
          anchors.verticalCenter: parent.verticalCenter
          width: parent.parent.width / 3
          first.value: 60; second.value: 120
          from: 40; to: 180
          stepSize: 5; snapMode: RangeSlider.SnapAlways
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          text: Math.floor(tempoRange.second.value); font { bold: true; pixelSize: NOO.factor() * 0.8 }
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
        }
        melListView.setLevel(creator.level())
        melListView.currentMelody = -1 // reset selection
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
    } // melody from set has melodies aready in the working level
  }
}
