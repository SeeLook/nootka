/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2017 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Window 2.0

import Nootka 1.0

Item {
  property Item toolBar: null // fake, for main window information
  property alias scoreMenu: scoreMenu

  x: Screen.pixelDensity / 2
  y: Screen.pixelDensity / 2
  z: 100
  width: nootkaWindow.fontSize * 1.5
  height: nootkaWindow.fontSize * 4

  Rectangle {
    id: bg
    anchors.fill: parent
    color: area.pressed ? activPal.highlight : Qt.rgba(0.2, 0.2, 0.2, 0.05)
    radius: nootkaWindow.fontSize / 3
    visible: false
  }
  DropShadow {
    anchors.fill: bg
    horizontalOffset: nootkaWindow.fontSize / 5
    verticalOffset: nootkaWindow.fontSize / 5
    radius: 8.0
    samples: 17
    color: activPal.shadow
    source: bg
  }

  Repeater {
    model: 3
    Rectangle {
      width: nootkaWindow.fontSize * 0.5
      height: nootkaWindow.fontSize * 0.5
      x: nootkaWindow.fontSize * 0.5
      y: nootkaWindow.fontSize * 0.625 * (index + 1) + index * nootkaWindow.fontSize * 0.5
      color: "black"
      radius: width / 2
    }
  }

  Drawer {
    id: mainDrawer
    width: nootkaWindow.fontSize * 16
    height: nootkaWindow.height
    onVisibleChanged: {
      nooLabel.bgColor = visible ? nooLabel.randColor() : "white"
    }
    Flickable {
      anchors.fill: parent
      clip: true
      contentHeight: drawerColumn.height
      Column {
        id: drawerColumn
        width: parent.width
        spacing: Screen.pixelDensity / 2
        NootkaLabel {
          id: nooLabel
          height: nootkaWindow.fontSize * 6.328125 // (logo ratio) 0.3955078125 * 16
          onClicked: {
            mainDrawer.close()
            nootkaWindow.aboutAct.trigger()
          }
        }
        MenuButton { action: nootkaWindow.levelAct; onClicked: mainDrawer.close() }
        MenuButton { action: nootkaWindow.examAct; /*onClicked: mainDrawer.close()*/ }
        MenuButton { action: nootkaWindow.settingsAct; onClicked: mainDrawer.close() }
        MenuButton { action: nootkaWindow.scoreAct }
        Column { // drop-down menu with score actions
          id: scoreMenu
          function open() { visible ? state = "Invisible" : state = "Visible" }
          spacing: Screen.pixelDensity / 2
          width: parent.width - nootkaWindow.fontSize / 2
          x: -parent.width
          visible: false
          MenuButton { action: score.extraAccidsAct; onClicked: mainDrawer.close() }
          MenuButton { action: score.showNamesAct; onClicked: mainDrawer.close() }
          MenuButton { action: score.zoomInAct; onClicked: mainDrawer.close() }
          MenuButton { action: score.zoomOutAct; onClicked: mainDrawer.close() }
          states: [ State { name: "Visible"; when: scoreMenu.visible }, State { name: "Invisible"; when: !scoreMenu.visible } ]

          transitions: [
            Transition {
              from: "Invisible"; to: "Visible"
              SequentialAnimation {
                PropertyAction { target: scoreMenu; property: "visible"; value: true }
                NumberAnimation { target: scoreMenu; property: "x"; to: nootkaWindow.fontSize / 2; duration: 300 }
              }
            },
            Transition {
              from: "Visible"; to: "Invisible"
              SequentialAnimation {
                NumberAnimation { target: scoreMenu; property: "x"; to: -parent.width; duration: 300 }
                PropertyAction { target: scoreMenu; property: "visible"; value: false }
              }
            }
          ]
        }
        MenuButton { onClicked: nootkaWindow.close(); action: Taction { icon: "exit"; text: qsTr("exit") } }
      }
    }
  }

  MouseArea {
    id: area
    anchors.fill: parent
    onClicked: mainDrawer.open()
  }
}
