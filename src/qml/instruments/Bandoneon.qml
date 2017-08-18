/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2

import Nootka 1.0


Item {
  property alias note: bando.note
  property alias instrBg: bando

  property real factor: height / 100

  width: childrenRect.width; height: parent ? parent.height : 0
  anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined

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

  TbandoneonBg {
    id: bando
    width: mainRow.width; height: mainRow.height; x: mainRow.x; y: mainRow.y
    z: 10
    rightX: factor * 200 + buttonCol.width
    opening: openButt.checked
    closing: closeButt.checked
  }

  Row {
    id: mainRow
    height: parent.height
    z: 5
    Rectangle {
      height: parent.height
      width: factor * 200
      color: "black"
      Repeater {
        model: 33
        Rectangle {
          width: parent.height / 8; height: width
          x: bando.xAt(index) * factor
          y: bando.yAt(index) * factor + width / 3
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
            onClicked: {
              if (!openButt.checked && !closeButt.checked)
                openButt.checked = true
              bando.currentIndex = index
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
        spacing: factor * 3
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
        width: nootkaWindow.fontSize * 10
        text: qsTr("bellows<br>opening")
        checkable: true
        background: Rectangle { color: openButt.checked ? "blue" : "gray" }
        onClicked: {
          if (openButt.checked)
            closeButt.checked = false
        }
      }
      Button {
        id: closeButt
        width: nootkaWindow.fontSize * 10
        text: qsTr("bellows<br>closing")
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
      width: factor * 190
      color: "black"
      Repeater {
        model: 38
        Rectangle {
          width: parent.height / 8; height: width
          x: bando.xAt(index + 33) * factor
          y: bando.yAt(index + 33) * factor + width
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
            onClicked: {
              if (!openButt.checked && !closeButt.checked)
                openButt.checked = true
              bando.currentIndex = index + 33
            }
          }
        }
      }
    }

  }

  OutScaleTip { visible: bando.outOfScale && !bando.active }

}

