/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


TipRect {
  id: noteName

  // private
  property int note: 0
  property int alter: 0
  property int octave: 0

//   color: activPal.window

  Column {
    z: 2
    anchors.fill: parent
    spacing: (noteName.height - childrenRect.height) / 4
    topPadding: spacing

    Rectangle {
      anchors.horizontalCenter: parent.horizontalCenter
      width: noteName.width * 0.9
      height: noteName.height * 0.2
      Text {
        y: -noteName.height * 0.17
        width: parent.width
        height: parent.height
        id: nameLabel
        font { pixelSize: height * 0.8; family: "Scorek" }
        horizontalAlignment: Text.AlignHCenter
      }
    }

    Row {
      z: 2
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: noteName.width / 56
      Repeater {
        model: 7
        Button {
          width: noteName.width / 8
          height: noteName.width * 0.15
          contentItem: Text {
            font.pixelSize: noteName.height * 0.1
            text: Noo.noteName(Noo.note(index + 1, 0, 0), 1, false)
          }
          onClicked: {
            noteName.note = index + 1
            buttonPressed()
          }
        }
      }
    }

    Row {
      z: 2
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: noteName.width / 56
      Repeater {
        model:  [ "B", "b", "#", "x" ]
        Button {
          width: noteName.width / 8
          height: noteName.width * 0.15
          contentItem: Text {
            font { family: "Nootka"; pixelSize: noteName.height * 0.1 }
            text: modelData
          }
          onClicked: {
            noteName.alter = index - 2
            buttonPressed()
          }
        }
      }
    }
  }

  MouseArea {
    z: 1
    anchors.fill: parent
    hoverEnabled: true
    onExited: noteName.visible = false
  }

  function buttonPressed() {
    nameLabel.text = Noo.noteName(Noo.note(note, octave, alter), 1)
  }
}
