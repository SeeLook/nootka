/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
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
      width: parent.width * (Noo.isAndroid() ? 0.98 : 1)
      readonly property string keyRangeText: "<br>" + qsTr("They will be transposed if key signatures are set to be used and any of them differs from the key(s) defined below.")
      property var descList: [ qsTr("Melodies are composed from a note range defined on the 'Range' page."),
                               qsTr("Melodies are composed from notes selected on the score below."),
                               qsTr("Melodies are selected from the list.") ]
      description: descList[melCombo.currentIndex] + (melCombo.currentIndex > 0 ? keyRangeText : "")
      Row {
        spacing: Noo.fontSize()
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
          text: Noo.TR("LevelCreator", "Melody")
          anchors.verticalCenter: parent.verticalCenter
          color: enabled ? activPal.text : disdPal.text
        }
        TcomboBox {
          id: melCombo
          width: Noo.fontSize() * 20
          model: [ qsTr("in selected range"), qsTr("from notes below"), qsTr("from the list") ]
          currentIndex: creator.randMelody
          onActivated: creator.randMelody = currentIndex
        }
      }
    }
    Grid {
      id: melGrid
      spacing: melPage.width / (Noo.isAndroid() ? 100 : 50)
      columns: Noo.fontSize() * 50 > melPage.width ? 1 : 2 // (melLenTile.visible ? 2 : 1)
      anchors.horizontalCenter: parent.horizontalCenter

      Tile {
        id: melLenTile
        visible: creator.isMelody && !creator.hasRhythms && melCombo.currentIndex !== 2
        description: qsTr("Maximum number of notes in a melody. Melody length is random value between 70% and 100% of that number.")
        anchors.horizontalCenter: undefined
        width: melGrid.columns === 1 ? melPage.width * 0.98 : Math.max(lenRow.width + Noo.fontSize() * 4, melPage.width * 0.48)
        Row {
          id: lenRow
          spacing: Noo.fontSize()
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
        width: melGrid.columns === 1 ? melPage.width * 0.98 : Math.max(checkBox.width + Noo.fontSize() * 4, melPage.width * 0.48)
        checked: creator.endsOnTonic
        checkBox.onClicked: creator.endsOnTonic = checked
      }

      Tile {
        width: melGrid.columns === 1 ? melPage.width * 0.98 : Math.max(randOrderChB.width + Noo.fontSize() * 4, melPage.width * 0.48)
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
        width: melGrid.columns === 1 ? melPage.width * 0.98 : Math.max(repeatRow.width + Noo.fontSize() * 4, melPage.width * 0.48)
        visible: melCombo.currentIndex === 2
        anchors.horizontalCenter: undefined
        description: qsTr("How many times during an exam a melody from the list has to be played or written correctly. Warning! It multiplies question number.")
        Row {
          id: repeatRow
          spacing: Noo.fontSize()
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
        spacing: Noo.fontSize()
        anchors.horizontalCenter: parent.horizontalCenter
        TcheckBox {
            id: inTempoChB
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Play in tempo")
        }
        Text {
          anchors.verticalCenter: parent.verticalCenter
          text: Math.floor(tempoRange.first.value); font { bold: true; pixelSize: Noo.fontSize() * 0.8 }
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
    } else if (melCombo.currentIndex === 2) { // random melody from set
        melListView.save()
    }
  }
}
