/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka.Charts 1.0
import Score 1.0
import "../score"
import "../"


TipRect {
  id: questTip

  property alias tipItem: tipItem

  width: tipItem.width; height: tipItem.height
  z: 5000
  x: Noo.bound(parent.height / 12, tipItem.pos.x - chartView.list.contentX, parent.width - width * 1.2)
  y: tipItem.pos.y > parent.height / 2 ? tipItem.pos.y - height - parent.height / 14 : tipItem.pos.y + parent.height / 20
  shadowRadius: Noo.fontSize() * 3; horizontalOffset: Noo.fontSize() / 3; verticalOffset: horizontalOffset

  visible: scale > 0

  border { color: tipItem.color; width: Noo.fontSize() / 7 }
  color: Qt.tint(activPal.base, Noo.alpha(tipItem.color, 50))
  radius: Noo.fontSize()

  scale: tipItem.show ? 1 : 0

  // overlay area to catch when mouse exits a tip and hide it
  MouseArea {
    id: overArea
    parent: questTip.parent
    anchors.fill: parent
    z: 4999
    visible: false
    hoverEnabled: true
    onEntered: {
      visible = false
      chartItem.tipExited()
    }
  }

  Behavior on scale { NumberAnimation { duration: 200 }}
  Behavior on x { NumberAnimation { duration: 200 }}
  Behavior on y { NumberAnimation { duration: 200 }}

  TchartTipItem {
    id: tipItem
    width: (lineCol.visible ? lineCol.width : Math.max(scoreRow.width, headRow.width)) + Noo.fontSize()
    height: (lineCol.visible ? lineCol.height : resultCol.y + resultCol.height) + Noo.fontSize()

    onQuestionWasSet: attemptSpin.value = 0

    Row {
      id: headRow
      y: Noo.fontSize() / 2
      visible: tipItem.tipType === 0
      spacing: Noo.fontSize() * 2
      anchors.horizontalCenter: parent.horizontalCenter
      Text {
        anchors.verticalCenter: parent.verticalCenter
        text: tipItem.number + "."
        font { bold: true; pixelSize: Noo.fontSize() * 1.6 }
        color: activPal.text
      }
      Text {
        anchors.verticalCenter: parent.verticalCenter
        text: tipItem.qaText; horizontalAlignment: Text.AlignHCenter
        font { pixelSize: Noo.fontSize() * 0.8 }
        color: activPal.text
      }
    }

    Row {
      id: scoreRow
      visible: tipItem.tipType === 0
      anchors { horizontalCenter: parent.horizontalCenter; top: headRow.bottom }
      Item {
        anchors.verticalCenter: parent.verticalCenter
        visible: tipItem.leftScoreVisible
        height: tipItem.isMelody ? tipItem.leftScoreHeight : Noo.fontSize() * 12
        width: Noo.fontSize() * (tipItem.isMelody ? 24 : 12)
        Score {
          y: tipItem.yScoreLeftOff
          width: parent.width; height: Noo.fontSize() * 12
          Component.onCompleted: {
            bgRect.destroy()
            tipItem.leftScore = scoreObj
          }
        }
      }
      Text {
        visible: !tipItem.leftScoreVisible; anchors.verticalCenter: parent.verticalCenter
        text: tipItem.questionText; textFormat: Text.RichText
        color: activPal.text
      }
      Text { // question mark, visible only for single note questions
        anchors.verticalCenter: parent.verticalCenter
        visible: !tipItem.isMelody
        text: "?"; font { pixelSize: Noo.fontSize() * 4; family: "Nootka" }
        color: GLOB.wrongColor
      }
      Text {
        visible: !tipItem.rightScoreVisible && !tipItem.isMelody; anchors.verticalCenter: parent.verticalCenter
        text: tipItem.answerText; textFormat: Text.RichText
        color: activPal.text
      }
      Item {
        height: tipItem.isMelody ? tipItem.rightScoreHeight : Noo.fontSize() * 12
        width: Noo.fontSize() * 12
        anchors.verticalCenter: parent.verticalCenter
        visible: tipItem.rightScoreVisible
        Score {
          y: tipItem.yScoreRightOff
          height: Noo.fontSize() * 12; width: Noo.fontSize() * 12
          Component.onCompleted: {
            bgRect.destroy()
            tipItem.secondScore = scoreObj
          }
        }
      }
    } // score row

    TspinBox {
      id: attemptSpin
      y: scoreRow.y + scoreRow.height - Noo.fontSize()
      anchors.horizontalCenter: parent.horizontalCenter
      visible: tipItem.isMelody && tipItem.tipType === 0
      width: Noo.fontSize() * 15
      font.pixelSize: Noo.fontSize() * 0.8
      from: 0; to: tipItem.attempts
      textFromValue: function(value) {
        return Noo.TR("Texam", "attempt") + " " + value + " " + qsTr("of", "It will give text: 'Attempt x of y'") + " " + to
      }
      function pressed(m) {
        if (m.x > width / 2)
          up.pressed = true
        else
          down.pressed = true
      }
      function released() {
        if (down.pressed || up.pressed) {
          var prevV = value
          if (up.pressed)
            increase()
          else
            decrease()
          if (prevV !== value)
            tipItem.setAttemptNr(value)
        }
        up.pressed = false
        down.pressed = false
      }
    }

    Column {
      id: resultCol
      anchors { horizontalCenter: parent.horizontalCenter; top: attemptSpin.visible ? attemptSpin.bottom : scoreRow.bottom }
      width: parent.width - Noo.fontSize(); topPadding: Noo.fontSize() / 2
      visible: tipItem.tipType === 0
      Text {
        width: parent.width; visible: text !== ""
        anchors.horizontalCenter: parent.horizontalCenter
        text: tipItem.attemptDetails(attemptSpin.value); textFormat: Text.StyledText
        color: activPal.text; horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap; font.pixelSize: Noo.fontSize() * 0.9
      }
      Text {
        width: parent.width; visible: text !== ""
        anchors.horizontalCenter: parent.horizontalCenter
        text: tipItem.attemptResult(attemptSpin.value); textFormat: Text.StyledText
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap; font.pixelSize: Noo.fontSize() * 0.9
      }
      Text {
        width: parent.width; visible: text !== ""
        anchors.horizontalCenter: parent.horizontalCenter
        text: attemptSpin.value === 0 ? tipItem.resultText : ""; textFormat: Text.StyledText
        color: tipItem.color; horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
      }
      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        text: tipItem.timeText; textFormat: Text.StyledText
        font { pixelSize: Noo.fontSize() * 0.9 }
        color: activPal.text; horizontalAlignment: Text.AlignHCenter
      }
    }

    Column { // line tip column
      id: lineCol
      padding: Noo.fontSize() / 2
      visible: tipItem.tipType === 1
      Text {
        width: Noo.fontSize() * 20
        anchors.horizontalCenter: parent.horizontalCenter
        color: activPal.text; textFormat: Text.RichText
        text: tipItem.tipText; horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
      }
    }
  } // tipItem

  MouseArea {
    id: tipArea
    anchors.fill: parent
    hoverEnabled: true
    onEntered: chartItem.tipEntered()
    onExited: overArea.visible = true
    onPressed: attemptSpin.pressed(mapToItem(attemptSpin, mouse.x, mouse.y))
    onReleased: attemptSpin.released()
  }
}
