/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9

import Nootka.Charts 1.0
import Score 1.0
import "../score"
import "../"


TipRect {
  id: questTip
  property alias tipItem: tipItem

  shadowRadius: Noo.fontSize()
  width: Noo.fontSize() * 23; height: tipCol.height
  z: 100; y: Math.max(10, Math.min(parent.height - height - 10, tipItem.pos.y - height / 2))
  x: tipItem.pos.x + (tipItem.pos.x > parent.width / 2 ? - (width + 20) : 10)
  visible: scale > 0

  border { color: tipItem.color; width: 1 }
  color: Qt.tint(activPal.base, Noo.alpha(tipItem.color, 50))

  scale: tipItem.show || tipArea.containsMouse ? 1 : 0
  Behavior on scale { NumberAnimation { duration: 200 }}

  TchartTipItem {
    id: tipItem
    width: parent.width

    Column {
      id: tipCol
      width: parent.width
      Column { // question tip column
        width: parent.width; padding: Noo.fontSize() / 4
        visible: tipItem.tipType === 0
        Row {
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
        Row { // score row
          anchors.horizontalCenter: parent.horizontalCenter
          Item {
            visible: tipItem.leftScoreVisible
            height: Noo.fontSize() * 10; width: Noo.fontSize() * (tipItem.isMelody ? 22.5 : 9)
            Score {
              width: parent.width; height: parent.height
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
          Loader {
            sourceComponent: tipItem.hasSecondScore ? scoreTwoComp : null; anchors.verticalCenter: parent.verticalCenter
            visible: tipItem.rightScoreVisible
          }
        }
        Text {
          anchors.horizontalCenter: parent.horizontalCenter
          text: tipItem.resultText; textFormat: Text.StyledText
          color: tipItem.color; horizontalAlignment: Text.AlignHCenter
        }
        Text {
          anchors.horizontalCenter: parent.horizontalCenter
          text: tipItem.timeText; textFormat: Text.StyledText
          font { pixelSize: Noo.fontSize() * 0.8 }
          color: activPal.text
        }
      }

      Column { // line tip column
        width: parent.width; padding: Noo.fontSize() / 2
        visible: tipItem.tipType === 1
        Text {
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.RichText
          text: tipItem.tipText; horizontalAlignment: Text.AlignHCenter
        }
      }
    }
  }

  MouseArea {
    id: tipArea
    anchors.fill: parent
    hoverEnabled: true
  }

  Component {
    id: scoreTwoComp
    Item {
      height: Noo.fontSize() * 10; width: Noo.fontSize() * 9
      Score {
        anchors.fill: parent
        Component.onCompleted: {
          bgRect.destroy()
          tipItem.secondScore = scoreObj
        }
      }
    }
  }
}
