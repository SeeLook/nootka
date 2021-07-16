/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Nootka.Main 1.0


TnameItem {
  id: noteName

  nameStyle: GLOB.noteNameStyle
  buttonNameStyle: GLOB.noteNameStyle

  property var score: null

  width: score.width; height: score.height
  x: nootkaWindow.topToBott ? score.x : score.width
  y: score.y + (nootkaWindow.topToBott ? score.height : 0)
  z: 4

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
  property real buttHeight: height / (NOO.isAndroid() ? 10 : 12)
  property real buttWidth: width / (NOO.isAndroid() ? 8 : 9)
  property var correctAnim: null

  Rectangle {
    anchors.fill: parent
    color: NOO.alpha(activPal.window, 230)
  }

  onCorrectName: {
    if (!correctAnim)
      correctAnim = Qt.createComponent("qrc:/exam/CorrectNameAnim.qml").createObject(noteName)
    correctAnim.doCross = textItem.text === ""
    correctAnim.start()
  }

  Rectangle {
    y: parent.height / 14
    anchors.horizontalCenter: parent.horizontalCenter
    width: noteName.width * 0.9; height: noteName.height / 5
    color: bgColor
    Text {
      visible: question
      x: parent.width - width
      text: "?"; color: GLOB.wrongColor
      font { family: "Nootka"; pixelSize: parent.height }
    }
    Text {
      id: textItem
      y: (parent.height - height / scale) / 2 - font.pixelSize * 0.3
      text: nameText; color: activPal.text
      width: parent.width - NOO.factor() / 2; anchors.horizontalCenter: parent.horizontalCenter
      horizontalAlignment: Text.AlignHCenter
      style: Text.Outline; styleColor: markColor
      font { pixelSize: Math.min(parent.height * 0.266, width * 0.05); family: "Scorek" }
      transformOrigin: Item.Center
      scale: 3 // scale to increase outline for marking answers
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
        text: noteButtonText(index + 1, buttonNameStyle, GLOB.seventhIsB)
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
    RectButton {
      anchors.verticalCenter: parent.verticalCenter
      height: buttHeight; yOffset: height * 0.2; y: NOO.factor() / 3
      onClicked: Qt.openUrlExternally(octavesLink())
      enabled: !disabled
      text: qsTranslate("TnameItem", "Octaves") + ":"
      textColor: hovered ? activPal.highlightedText : activPal.highlight
      font { pixelSize: buttHeight * 0.4; bold: true }
      statusTip: octavesLinkStatus(); statusPos: Item.Top
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
        font { pixelSize: height * (GLOB.scientificOctaves ? 0.8 : 0.5); bold: true }
        text: GLOB.scientificOctaves ? index * 2 + 1 : octaveName(index * 2 - 2)
        onClicked: octave = index * 2 - 2
        hoverEnabled: !NOO.isAndroid() && GLOB.showHints
        onHoveredChanged: NOO.setStatusTip(hovered ? octaveStatusTip(index * 2 - 2) : "")
        onPressAndHold: NOO.setStatusTip(octaveStatusTip(index * 2 - 2))
        onReleased: {
          if (NOO.isAndroid())
            NOO.setStatusTip("")
        }
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
        font { pixelSize: height * (GLOB.scientificOctaves ? 0.8 : 0.5); bold: true }
        text: GLOB.scientificOctaves ? index * 2 : octaveName(index * 2 - 3)
        onClicked: octave = index * 2 - 3
        hoverEnabled: !NOO.isAndroid() && GLOB.showHints
        onHoveredChanged: NOO.setStatusTip(hovered ? octaveStatusTip(index * 2 - 3) : "")
        onPressAndHold: NOO.setStatusTip(octaveStatusTip(index * 2 - 3))
        onReleased: {
          if (NOO.isAndroid())
            NOO.setStatusTip("")
        }
      }
    }
  }

}


