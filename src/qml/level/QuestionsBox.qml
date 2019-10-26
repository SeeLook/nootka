/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import "../"


Tile {
  property int qId: 0
  property alias questionText: questionChB.text
  property alias questionChecked: questionChB.checked
  property int answerBits: 0

  property var answSymb: [ "s", "c", GLOB.instrument.glyph, "n" ]

  width: col.width + Noo.fontSize() * 3
//   anchors.horizontalCenter: undefined

  description: unfold.checked ? qsTr("Select the type of answers for this kind of question.") : ""

  Column {
    id: col
    anchors.horizontalCenter: parent.horizontalCenter
    Row {
      anchors.horizontalCenter: parent.horizontalCenter
      spacing: Noo.fontSize() / 2
      TcheckBox {
        id: questionChB
        anchors.verticalCenter: parent.verticalCenter
        onClicked: creator.questionAs = checked ? creator.questionAs | Math.pow(2, qId) : creator.questionAs & ~Math.pow(2, qId)
      }
      Text {
        text: answSymb[qId] + "?"
        anchors.verticalCenter: parent.verticalCenter
        font { family: "nootka"; pixelSize: Noo.fontSize() * 2.5 }
        color: activPal.text
      }
      Text {
        property bool checked: false
        id: unfold
        anchors.verticalCenter: parent.verticalCenter
        font { pixelSize: Noo.fontSize() * 2; bold: true }
        color: ma.containsMouse ? activPal.highlight : activPal.text
        text: "   ⋮"
        MouseArea {
          id: ma
          hoverEnabled: true
          anchors.fill: parent
          onClicked: unfold.checked = !unfold.checked
        }
      }
    }
    Row {
      spacing: Noo.fontSize() / 2
      anchors.horizontalCenter: parent.horizontalCenter
      Text { text: qsTr("answers") + ":"; anchors.verticalCenter: parent.verticalCenter; color: activPal.text }
      Grid {
        spacing: Noo.fontSize() / 2
        columns: unfold.checked ? 1 : 4
        Repeater {
          model: 4
          Row {
            visible: (index !== 2 || qId !== 2) || GLOB.instrument.isGuitar
            property alias checked: aChB.checked
            layoutDirection: unfold.checked ? Qt.RightToLeft : Qt.LeftToRight
            spacing: Noo.fontSize() / 2
            TcheckBox {
              id: aChB
              text: unfold.checked ? Noo.qaTypeText(index) : ""
              checked: answerBits & Math.pow(2, index)
              onClicked: creator.setAnswers(qId, checked ? answerBits | Math.pow(2, index) : answerBits & ~Math.pow(2, index))
            }
            Text {
              font { family: "nootka"; pixelSize: Noo.fontSize() * 2.5 }
              text: answSymb[index]
              width: unfold.checked ? Noo.fontSize() * 3 : undefined
              horizontalAlignment: Text.AlignHCenter
              color: activPal.text
            }
          }
        }
      }
    }
  }
}
