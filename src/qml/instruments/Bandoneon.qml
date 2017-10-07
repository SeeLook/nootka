/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

import Nootka 1.0
import "../"


TbandoneonBg {
  property real factor: height / 100

  // private
  property int hiId: -1

  x: (parent.width - width) / 2
  width: mainRow.width
  height: parent ? parent.height : 0

//   Image {
//     source: Noo.pix("bandoneon-left")
//     width: factor * 260
//     y: -factor * 100
//     x: -factor * 40
//     z: 1
//   }
//   Image {
//     source: Noo.pix("bandoneon-right")
//     width: factor * 240
//     y: -factor * 70
//     x: factor * 240
//     z: 2
//   }

  rightX: factor * 210 + buttonCol.width
  opening: openButt.checked
  closing: closeButt.checked

  Row {
    id: mainRow
    height: parent.height
    z: 5
    Rectangle {
      height: parent.height
      width: factor * 210
      color: "black"
      Repeater {
        model: 33
        Rectangle {
          width: parent.height / 8; height: width
          x: xAt(index) * factor
          y: yAt(index) * factor + width
          color: "white"
          radius: width / 2
//           Text {
//             anchors.fill: parent
//             horizontalAlignment: Text.AlignHCenter
//             verticalAlignment: Text.AlignVCenter
//             text: index
//           }
          MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: hiId = index
            onExited: hiId = -1
            onClicked: {
              if (!openButt.checked && !closeButt.checked)
                openButt.checked = true
              currentIndex = index
            }
          }
        }
      }
    }

    Column {
      id: buttonCol
      anchors.bottom: parent.bottom
      spacing: factor * 4
      bottomPadding: factor
      Row {
        anchors.horizontalCenter: parent.horizontalCenter
        y: factor * 2
        spacing: factor * 1.5
        Text {
          text: openButt.checked ? "<" : ">"
          color: openButt.checked ? "blue" : "red"
          font.pixelSize: factor * 15
          visible: openButt.checked || closeButt.checked
        }
        Repeater {
          model: 5
          Rectangle {
            antialiasing: true
            visible: (index !== 0 && index !== 4) || closeButt.checked
            width: factor * 2
            radius: factor
            height: factor * 15
            color: openButt.checked ? "blue" : (closeButt.checked ? "red" : "gray")
            y: Math.abs(index - 2) * factor
            transformOrigin: Item.Bottom
            rotation: (index - 2) * 10
          }
        }
        Text {
          text: openButt.checked ? ">" :  "<"
          color: openButt.checked ? "blue" : "red"
          font.pixelSize: factor * 15
          visible: openButt.checked || closeButt.checked
        }
      }
      Button {
        id: openButt
        width: factor * 40
        height: Noo.fontSize() * 3
        contentItem: TlabelText { 
          text: qsTr("bellows<br>opening"); width: parent.width
          horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
        }
        checkable: true
        background: Rectangle { color: openButt.checked ? "blue" : "gray" }
        onClicked: {
          if (openButt.checked)
            closeButt.checked = false
        }
      }
      Button {
        id: closeButt
        width: factor * 40
        height: Noo.fontSize() * 3
        contentItem: TlabelText {
          text: qsTr("bellows<br>closing"); width: parent.width
          horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
        }
        checkable: true
        background: Rectangle { color: closeButt.checked ? "red" : "gray" }
        onClicked: {
          if (closeButt.checked)
            openButt.checked = false
        }
      }
    }

    Rectangle {
      height: parent.height
      width: factor * 220
      color: "black"
      Repeater {
        model: 38
        Rectangle {
          width: parent.height / 8; height: width
          x: xAt(index + 33) * factor * 1.2
          y: yAt(index + 33) * factor + width
          color: "white"
          radius: width / 2
//           Text {
//             anchors.fill: parent
//             horizontalAlignment: Text.AlignHCenter
//             verticalAlignment: Text.AlignVCenter
//             text: index + 33
//           }
          MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: hiId = index + 33
            onExited: hiId = -1
            onClicked: {
              if (!openButt.checked && !closeButt.checked)
                openButt.checked = true
              currentIndex = index + 33
            }
          }
        }
      }
    }

  }

  Rectangle {
    id: hi
    color: GLOB.fingerColor
    width: parent.height / 8
    height: width
    radius: width / 2
    x: hiId > -1 ? (hiId > 32 ? rightX : 0) + xAt(hiId) * factor * (hiId > 32 ? 1.2 : 1) : 0
    y: hiId > -1 ? yAt(hiId) * factor + width : 0
    visible: false
  }

  DropShadow {
    id: fingerShadow
    anchors.fill: hi
    horizontalOffset: factor * 2
    verticalOffset: factor * 2
    color: activPal.shadow
    radius: 8.0
    source: hi
    z: 20
    visible: hiId > -1
  }

  OutScaleTip { visible: outOfScale && !active }

}

