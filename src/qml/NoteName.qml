/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2018 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.Main 1.0


TnameItem {
  id: noteName

  nameStyle: GLOB.noteNameStyle
  buttonNameStyle: GLOB.noteNameStyle

  property var score: null
  width: score.width; height: score.height
  x: score.width + 1; y: score.y; z: 5
  onNoteChanged: {
    if (!GLOB.isExam)
      score.setNote(score.scoreObj.note(0), noteName.note)
  }

  Connections {
    target: score
    enabled: !GLOB.isExam
    onNoteChanged: noteName.note = score.note
  }

  // private
  property real buttHeight: height / 12
  property real buttWidth: width / 9

  Rectangle {
    anchors.fill: parent
    color: Noo.alpha(activPal.window, 230)
  }

  Rectangle {
    y: parent.height / 14
    anchors.horizontalCenter: parent.horizontalCenter
    width: noteName.width * 0.9
    height: noteName.height / 5
    color: bgColor
    Row {
      anchors.horizontalCenter: parent.horizontalCenter
      height: parent.height
      spacing: Noo.fontSize()
      Text {
        y: -noteName.height * 0.17
        text: nameText; color: activPal.text
        height: parent.height
        style: Text.Outline; styleColor: markColor
        font { pixelSize: height * 0.8; family: "Scorek" }
      }
      Text {
        font { pixelSize: parent.height * 0.9; family: "Nootka" }
        text: appendix; color: GLOB.wrongColor
        anchors.verticalCenter: parent.verticalCenter
      }
    }
  }

  ButtonGroup { id: stepsGr }

  Row {
    y: parent.height * 0.34
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: noteName.width / 54
    Repeater {
      id: stepRep
      model: 7
      TcuteButton {
        enabled: !disabled
        width: buttWidth
        height: buttHeight
        checkable: true
        checked: !disabled && index === step - 1
        ButtonGroup.group: stepsGr
        font { pixelSize: height * 0.8; family: "Serif"; bold: true }
        text: noteButtonText(index + 1, buttonNameStyle)
        onClicked: { step = index + 1; noteButtonClicked() }
      }
    }
  }

  Row {
    y: parent.height / 2
    anchors.horizontalCenter: parent.horizontalCenter
    spacing: noteName.width / 56
    Repeater {
      model:  [ "B", "b", "#", "x" ]
      TcuteButton {
        enabled: !disabled
        visible: GLOB.enableDoubleAccids || (index > 0 && index < 3)
        width: buttWidth
        height: buttHeight * 1.1
        checkable: true
        checked: !disabled && ((index < 2 && alter === index - 2) || (index > 1 && alter === index - 1))
        font { family: "Nootka"; pixelSize: height * 0.85 }
        text: modelData
        onClicked: alter = checked ? (index < 2 ? index - 2 : index - 1) : 0
      }
    }
  }

  ButtonGroup { id: octavesGr }

  Row {
    y: parent.height * 0.65
    leftPadding: parent.width / 60
    spacing: parent.width / 20
    LinkText {
      enabled: !disabled
      text: octavesLink()
      font { pixelSize: buttHeight * 0.4; family: "Sans"; bold: true }
      anchors.verticalCenter: parent.verticalCenter
    }
    Repeater {
      model: 4
      TcuteButton {
        enabled: !disabled
        width: buttWidth * 1.2
        height: buttHeight
        checkable: true
        checked: !disabled && index === (octave + 2) / 2
        ButtonGroup.group: octavesGr
        font { pixelSize: height * 0.5; family: "Sans"; bold: true }
        text: octaveName(index * 2 - 2)
        onClicked: octave = index * 2 - 2
      }
    }
  }

  Row {
    y: parent.height * 0.78
    leftPadding: spacing
    spacing: buttWidth * 0.64
    Repeater {
      model: 4
      TcuteButton {
        enabled: !disabled
        width: buttWidth * 1.2
        height: buttHeight
        checkable: true
        checked: !disabled && index === (octave + 3) / 2
        ButtonGroup.group: octavesGr
        font { pixelSize: height * 0.5; family: "Sans"; bold: true }
        text: octaveName(index * 2 - 3)
        onClicked: octave = index * 2 - 3
      }
    }
  }

}


