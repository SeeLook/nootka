/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka.name 1.0


TnameItem {
  id: noteName

  nameStyle: GLOB.noteNameStyle

  property var score: null
  width: score.width; height: score.height
  x: score.width + 1
  note: score.note
  onNoteChanged: score.setNote(score.scoreObj.note(0), noteName.note)

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
    color: Noo.alpha(activPal.base, 230)
    Text {
      y: -noteName.height * 0.17
      text: nameText
      width: parent.width
      height: parent.height
      font { pixelSize: height * 0.8; family: "Scorek" }
      horizontalAlignment: Text.AlignHCenter
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
        width: buttWidth
        height: buttHeight
        checkable: true
        checked: index === step - 1
        ButtonGroup.group: stepsGr
        font { pixelSize: height * 0.8; family: "Serif"; bold: true }
        text: noteButtonText(index + 1, nameStyle)
        onClicked: step = index + 1
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
        visible: GLOB.enableDoubleAccids || (index > 0 && index < 3)
        width: buttWidth
        height: buttHeight * 1.1
        checkable: step > 0
        checked: step > 0 && ((index < 2 && alter === index - 2) || (index > 1 && alter === index - 1))
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
    Text {
      text: octavesLink()
      font { pixelSize: buttHeight * 0.4; family: "Sans"; bold: true }
      anchors.verticalCenter: parent.verticalCenter
      onLinkActivated: Qt.openUrlExternally(link)
      MouseArea { // make hand cursor over link text
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
      }
    }
    Repeater {
      model: 4
      TcuteButton {
        width: buttWidth * 1.2
        height: buttHeight
        checkable: step > 0
        checked: index === (octave + 2) / 2
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
        width: buttWidth * 1.2
        height: buttHeight
        checkable: step > 0
        checked: index === (octave + 3) / 2
        ButtonGroup.group: octavesGr
        font { pixelSize: height * 0.5; family: "Sans"; bold: true }
        text: octaveName(index * 2 - 3)
        onClicked: octave = index * 2 - 3
      }
    }
  }

}


