/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2021 by Tomasz Bojczuk (seelook@gmail.com)     *
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
  x: NOO.bound(parent.height / 12, tipItem.pos.x - chartView.list.contentX, parent.width - width * 1.2)
  y: tipItem.pos.y > parent.height / 2 ? tipItem.pos.y - height - parent.height / 14 : tipItem.pos.y + parent.height / 20
  shadowRadius: NOO.factor() * 3; horizontalOffset: NOO.factor() / 3; verticalOffset: horizontalOffset

  visible: scale > 0

  border { color: tipItem.color; width: NOO.factor() / 7 }
  color: Qt.tint(activPal.base, NOO.alpha(tipItem.color, 50))
  radius: NOO.factor()

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
    width: (lineCol.visible ? lineCol.width : Math.max(scoreRow.width, headRow.width)) + NOO.factor()
    height: (lineCol.visible ? lineCol.height : resultCol.y + resultCol.height) + NOO.factor()

    Row {
      id: headRow
      y: NOO.factor() / 2
      visible: tipItem.tipType === 0
      spacing: NOO.factor() * 2
      anchors.horizontalCenter: parent.horizontalCenter
      Text {
        anchors.verticalCenter: parent.verticalCenter
        text: tipItem.number + "."
        font { bold: true; pixelSize: NOO.factor() * 1.6 }
        color: activPal.text
      }
      Text {
        anchors.verticalCenter: parent.verticalCenter
        text: tipItem.qaText; horizontalAlignment: Text.AlignHCenter
        font { pixelSize: NOO.factor() * 0.8 }
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
        height: NOO.factor() * 10; width: NOO.factor() * 10
        Score {
          y: tipItem.yScoreLeftOff / 2
          scale: height / firstStaff.linesCount
          width: parent.width; height: NOO.factor() * 12
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
        text: "?"; font { pixelSize: NOO.factor() * 4; family: "Nootka" }
        color: GLOB.wrongColor
      }
      Text {
        visible: !tipItem.rightScoreVisible; anchors.verticalCenter: parent.verticalCenter
        text: tipItem.answerText; textFormat: Text.RichText
        color: activPal.text
      }
      Item {
        height: NOO.factor() * 10; width: NOO.factor() * 10
        anchors.verticalCenter: parent.verticalCenter
        visible: tipItem.rightScoreVisible
        Score {
          y: tipItem.yScoreRightOff / 2
          width: parent.width; height: NOO.factor() * 12
          scale: height / firstStaff.linesCount
          Component.onCompleted: {
            bgRect.destroy()
            tipItem.secondScore = scoreObj
          }
        }
      }
    } // score row

    Column {
      id: resultCol
      anchors { horizontalCenter: parent.horizontalCenter; top: scoreRow.bottom }
      width: parent.width - NOO.factor(); topPadding: NOO.factor() / 2
      visible: tipItem.tipType === 0
      Text {
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        text: tipItem.resultText; textFormat: Text.StyledText
        color: tipItem.color; horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
      }
      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        text: tipItem.timeText; textFormat: Text.StyledText
        font { pixelSize: NOO.factor() * 0.9 }
        color: activPal.text; horizontalAlignment: Text.AlignHCenter
      }
    }

    Column { // line tip column
      id: lineCol
      padding: NOO.factor() / 2
      visible: tipItem.tipType === 1
      Text {
        width: NOO.factor() * 20
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
  }
}
