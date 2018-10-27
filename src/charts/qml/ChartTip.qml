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
  visible: scale > 0 //tipItem.show || tipArea.containsMouse

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
            height: Noo.fontSize() * 10; width: tipItem.isMelody ? questTip.width - Noo.fontSize() / 2 : Noo.fontSize() * 7
            Score {
              anchors.fill: parent
              Component.onCompleted: {
                bgRect.destroy()
                tipItem.leftScore = scoreObj
              }
            }
          }
          Loader { sourceComponent: tipItem.isMelody ? null : qComp; anchors.verticalCenter: parent.verticalCenter }
          Loader { sourceComponent: tipItem.hasSecondScore ? scoreTwoComp : null; anchors.verticalCenter: parent.verticalCenter }
  //         Text {
  //           
  //         }
        }
        Text {
          anchors.horizontalCenter: parent.horizontalCenter
          text: tipItem.resultText; textFormat: Text.StyledText
          color: tipItem.color
        }
        Text {
          anchors.horizontalCenter: parent.horizontalCenter
          text: tipItem.timeText; textFormat: Text.StyledText
          font { pixelSize: Noo.fontSize() * 0.8 }
          color: activPal.text
        }
      }

      Column {
        width: parent.width; padding: Noo.fontSize() / 4
        visible: tipItem.tipType === 1
        Text {
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text
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
    id: qComp
    Text {
      visible: !tipItem.isMelody
      text: "?"; font { pixelSize: Noo.fontSize() * 4; family: "Nootka" }
      color: GLOB.wrongColor
    }
  }
  Component {
    id: scoreTwoComp
    Item {
      height: Noo.fontSize() * 10; width: Noo.fontSize() * 7
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
