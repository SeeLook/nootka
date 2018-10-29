/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import Nootka 1.0
import Nootka.Charts 1.0
import ".."


Window {
  id: analyzeWindow

  property alias exam: chartItem.exam
  property alias allowOpen: chartItem.allowOpen

  visible: true
  modality: Qt.WindowModal
  title: qsTr("Analyze")
  width: nootkaWindow.width; height: nootkaWindow.height; x: nootkaWindow.x; y: nootkaWindow.y

  Column {
    ToolBar {
      id: toolBar
      z: 255
      height: examActButt.height; width: analyzeWindow.width
      background: TipRect { anchors.fill: parent; color: activPal.window; radius: 0 }
      Row {
        ChartToolButton {
          id: examActButt
          visible: allowOpen
          taction: Taction {
            text: Noo.TR("QShortcut", "Open"); icon: "nootka-exam"
            onTriggered: menu.open()
          }
        }
        ChartToolButton {
          taction: Taction {
            text: Noo.TR("QShortcut", "Zoom In"); icon: "zoom-in"
            onTriggered: chartItem.zoom(true)
          }
        }
        ChartToolButton {
          taction: Taction {
            text: Noo.TR("QShortcut", "Zoom Out"); icon: "zoom-out"
            onTriggered: chartItem.zoom(false)
          }
        }
        ChartToolButton {
          taction: Taction {
            text: qsTr("linear chart"); icon: "linearChart"
//             onTriggered: 
          }
        }
        ChartToolButton {
          taction: Taction {
            text: qsTr("bar chart"); icon: "barChart"
//             onTriggered: 
          }
        }
        ChartToolButton {
          taction: Taction {
            text: Noo.TR("QMdiSubWindow", "Maximize"); icon: "fullscreen"
            onTriggered: visibility = visibility === 2 ? 4 : 2
          }
        }
        Item { height: 2; width: analyzeWindow.width / 100 }
        Column {
          visible: chartItem.isMelody
          spacing: toolBar.height / 20
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Y value:")
            font { pixelSize: toolBar.height / 4 }
            color: activPal.text
          }
          ComboBox {
            id: yCombo
            width: analyzeWindow.width / 7
            font { pixelSize: toolBar.height / 4 }
            model: chartItem.yValueActions
            delegate: ItemDelegate { text: modelData }
            onActivated: chartItem.changeChartYvalue(index)
          }
        }
        Item { height: 2; width: analyzeWindow.width / 100 }
        Column {
          spacing: toolBar.height / 20
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("ordered by:", "Keep a proper form - whole sentence will be: ordered by: question number, key signature, etc...")
            font { pixelSize: toolBar.height / 4 }
            color: activPal.text
          }
          ComboBox {
            id: orderCombo
            width: analyzeWindow.width / 7
            font { pixelSize: toolBar.height / 4 }
            model: [ qsTr("question number", "see comment in 'ordered by:' entry"), qsTr("note pitch"), qsTr("fret number"),
                     qsTr("key signature"), ("accidentals"), qsTr("question type"), qsTr("mistake") ]
            delegate: ItemDelegate { text: modelData }
          }
        }
        Item { height: 2; width: analyzeWindow.width / 100 }
        Column {
          spacing: toolBar.height / 20
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("student name:")
            font { pixelSize: toolBar.height / 4 }
            color: activPal.text
          }
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: chartItem.userName === "" ? "-----" : chartItem.userName
            font { pixelSize: toolBar.height / 3; bold: true }
            color: activPal.text
          }
        }
        Item { height: 2; width: analyzeWindow.width / 100 }
        Column {
          spacing: toolBar.height / 20
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: chartItem.questionCount
            font { pixelSize: toolBar.height / 4 }
            color: activPal.text; horizontalAlignment: Text.AlignHCenter
          }
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: chartItem.effectiveness
            font { pixelSize: toolBar.height / 4 }
            color: activPal.text; horizontalAlignment: Text.AlignHCenter
          }
        }
      }
      ChartToolButton {
        anchors.right: parent.right
        taction: Taction {
          text: Noo.TR("QPlatformTheme", "Close"); icon: "exit"
          onTriggered: close()
        }
      }
    }
    Flickable {
      id: chartFlick
      clip: true
      ScrollBar.vertical: ScrollBar { active: true; visible: true }
      ScrollBar.horizontal: ScrollBar { active: true; visible: true}
      width: analyzeWindow.width; height: analyzeWindow.height - toolBar.height
      contentWidth: chartItem.width; contentHeight: chartItem.height
      TchartItem {
        id: chartItem
        parentHeight: chartFlick.height
        tipItem: tip.tipItem
        ChartTip { id: tip }
      }
    }
  }

  Tmenu {
    id: menu
    x: examActButt.width; y: toolBar.height / 2
    height: Math.min(analyzeWindow.height - toolBar.height, contentItem.contentHeight); width: Noo.fontSize() * 20

    contentItem: ListView {
      clip: true
      ScrollBar.vertical: ScrollBar { active: true }
      model: chartItem.recentExamsActions
      delegate: MenuButton {
        action: modelData
        width: Noo.fontSize() * 20
        onClicked: menu.close()
        Rectangle { width: parent.width; height: index === chartItem.recentExamsActions.length - 1 ? 0 : 1; color: activPal.text; y: parent.height - 1;  }
      }
    }
  }

  onClosing: analyzeWindow.destroy()
}
