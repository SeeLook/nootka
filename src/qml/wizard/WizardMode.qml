/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2021 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Score 1.0
import "../"
import "../score"


Tflickable {
  id: wizardMode

  function setOptions() {
    if (modeGr.checkedButton === singleRadio)
      GLOB.singleNoteMode = true
    else if (modeGr.checkedButton === simpleRadio)
      GLOB.rhythmsEnabled = false
  }

  height: parent.height
  contentWidth: width; contentHeight: modeCol.height + NOO.factor() * 2

  ButtonGroup { id: modeGr }

  Column {
    id: modeCol
    anchors.horizontalCenter: parent.horizontalCenter

    LinkText {
      width: wizardMode.width - NOO.factor()
      wrapMode: Text.WordWrap; horizontalAlignment: Text.AlignHCenter
      text: qsTr("In main Nootka window score can be displayed in a few different ways, depends on experience level.")
                .replace(", ", ",<br>")
      font { pixelSize: NOO.factor() * (NOO.isAndroid() ? 1.2 : 1.5); bold: true }
    }

    Tile {
      width: wizardMode.width - NOO.factor()
      bgColor: singleRadio.checked ? nootkaWindow.bgColor : activPal.base
      bgBorder { width: 2; color: singleRadio.checked ? nootkaWindow.labelColor : activPal.text }
      Column {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: NOO.factor() * (NOO.isAndroid() ? 0.25 : 1)
        TradioButton {
          id: singleRadio
          anchors.horizontalCenter: parent.horizontalCenter
          text: " <b>" + NOO.TR("ScorePage", "use single note only") + "</b>"
          checked: true; textScale: NOO.isAndroid() ? 1 : (checked ? 1.3 : 1.1)
          ButtonGroup.group: modeGr
        }
        LinkText {
          anchors.horizontalCenter: parent.horizontalCenter
          text: qsTr("For beginners, when you don't know musical notation at all.")
        }
        GlowRect {
          anchors.horizontalCenter: parent.horizontalCenter
          z: 1; color: activPal.window; radius: NOO.factor() / 2
          width: singleRow.width + NOO.factor(); height: singleRow.height + NOO.factor()
          Row {
            id: singleRow
            anchors.centerIn: parent
            spacing: NOO.factor() * 2
            Rectangle {
              color: activPal.base
              width: singleScore.width; height: singleScore.height * 0.5
              Score {
                id: singleScore
                height: Math.max(NOO.factor() * 15, nootkaWindow.height * 0.3); width: Math.max(NOO.factor() * 10, nootkaWindow.height * 0.16)
                scale: height / firstStaff.linesCount
                y: -height * 0.3
                bgColor: "transparent"
                scoreObj.editMode: false
                meter: Tmeter.NoMeter
                Component.onCompleted: {
                  addNote(NOO.note(3, 1, -1, 0))
                }
              }
            }
            Column {
              spacing: NOO.factor() / 2
              Rectangle {
                color: activPal.base
                width: wizardMode.width * 0.3; height: singleScore.height * 0.2
                LinkText {
                  anchors.horizontalCenter: parent.horizontalCenter
                  y: -height * 0.25
                  text: NOO.styledName(NOO.note(3, 1, -1), GLOB.noteNameStyle)
                  font { family: "Scorek"; pixelSize: parent.height * 0.8 }
                }
              }
              Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: NOO.factor() / 2
                Repeater {
                  model: 7
                  TcuteButton {
                    anchors.verticalCenter: parent.verticalCenter
                    width: wizardMode.width * 0.032; height: width * 0.7
                    checkable: true; checked: index === 2
                    font { pixelSize: height * 0.8; bold: true }
                    text: NOO.noteName(NOO.note(index + 1, 1, 0), GLOB.noteNameStyle, false)
                  }
                }
              }
              Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: NOO.factor()
                Repeater {
                  model:  [ "b", "#" ]
                  TcuteButton {
                    anchors.verticalCenter: parent.verticalCenter
                    width: wizardMode.width * 0.035; height: width * 0.7
                    checkable: true; checked: index === 0
                    font { pixelSize: height * 0.85; family: "Nootka" }
                    text: modelData
                  }
                }
              }
            }
          }
        }
      }
      MouseArea {
        anchors.fill: parent
        onClicked: singleRadio.checked = true
      }
    }
    Tile {
      width: wizardMode.width - NOO.factor()
      bgColor: simpleRadio.checked ? nootkaWindow.bgColor : activPal.base
      bgBorder { width: 2; color: simpleRadio.checked ? nootkaWindow.labelColor : activPal.text }
      Column {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: NOO.factor() * (NOO.isAndroid() ? 0.25 : 1)
        TradioButton {
          id: simpleRadio
          anchors.horizontalCenter: parent.horizontalCenter
          text: " <b>" + qsTr("score without rhythms") + "</b>"
          ButtonGroup.group: modeGr; textScale: NOO.isAndroid() ? 1 : (checked ? 1.3 : 1.1)
        }
        LinkText {
          width: wizardMode.width - NOO.factor() * 3; wrapMode: Text.WordWrap
          horizontalAlignment: Text.AlignHCenter
          anchors.horizontalCenter: parent.horizontalCenter
          text: qsTr("When you already know musical notation a little but you cannot play your instrument fluently yet.")
        }
        GlowRect {
          color: activPal.base
          width: simpleScore.width; height: simpleScore.height * 0.6; radius: NOO.factor() / 2
          z: 1
          anchors.horizontalCenter: parent.horizontalCenter
          Score {
            id: simpleScore
            height: Math.max(NOO.factor() * 8, nootkaWindow.height * 0.25); width: height * 1.8
            y: -height * 0.25
            bgColor: "transparent"
            scoreObj.editMode: false; scoreObj.showNoteNames: true
            scoreObj.nameColor: GLOB.nameColor; scoreObj.nameStyle: GLOB.noteNameStyle
            meter: Tmeter.NoMeter
            Component.onCompleted: {
              for (var n = 1; n < 8; ++n)
                addNote(NOO.note(n, 1, 0, 0))
              addNote(NOO.note(1, 2, 0, 0))
            }
          }
        }
      }
      MouseArea {
        anchors.fill: parent
        onClicked: simpleRadio.checked = true
      }
    }
    Tile {
      width: wizardMode.width - NOO.factor()
      bgColor: fullRadio.checked ? nootkaWindow.bgColor : activPal.base
      bgBorder { width: 2; color: fullRadio.checked ? nootkaWindow.labelColor : activPal.text }
      Column {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: NOO.factor() * (NOO.isAndroid() ? 0.25 : 1)
        TradioButton {
          id: fullRadio
          z: 2
          anchors.horizontalCenter: parent.horizontalCenter
          text: " <b>" + qsTr("full notation") + "</b>"
          ButtonGroup.group: modeGr; textScale: NOO.isAndroid() ? 1 : (checked ? 1.3 : 1.1)
        }
        GlowRect {
          color: activPal.base
          z: 1
          width: fullScore.width; height: fullScore.height * 0.6; radius: NOO.factor() / 2
          anchors.horizontalCenter: parent.horizontalCenter
          Score {
            id: fullScore
            height: Math.max(NOO.factor() * 8, nootkaWindow.height * 0.25); width: height * 1.8
            y: -height * 0.25
            bgColor: "transparent"
            scoreObj.editMode: false; scoreObj.showNoteNames: true
            scoreObj.nameColor: GLOB.nameColor; scoreObj.nameStyle: GLOB.noteNameStyle
            Component.onCompleted: {
              addNote(NOO.note(1, 1, 0, 4))
              addNote(NOO.note(0, 1, 0, 4, true))
              addNote(NOO.note(2, 1, 0, 4))
              addNote(NOO.note(3, 1, 0, 4))
              addNote(NOO.note(4, 1, 0, 5))
              addNote(NOO.note(5, 1, 0, 5))
              addNote(NOO.note(6, 1, 0, 5))
              addNote(NOO.note(7, 1, 0, 5))
              addNote(NOO.note(1, 2, 0, 1))
            }
          }
        }
      }
      MouseArea {
        anchors.fill: parent
        onClicked: fullRadio.checked = true
      }
    }
  }
}
