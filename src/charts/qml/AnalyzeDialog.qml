/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2019 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

import Nootka 1.0
import Nootka.Charts 1.0
import "../"


Window {
  id: analyzeWindow

  property alias exam: chartItem.exam
  property alias allowOpen: chartItem.allowOpen

  /** private */
  property var helpTip: null
  property int upTextSize: Math.round(toolBar.height * 0.25)

  function openExam() {
    if (allowOpen)
      chartItem.openExam()
  }

  visible: true
  modality: Qt.WindowModal
  title: chartItem.chartWindowTitle
  width: nootkaWindow.width; height: nootkaWindow.height; x: nootkaWindow.x; y: nootkaWindow.y

  ButtonGroup { id: chartTypeGr }

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
            text: Noo.TR("QShortcut", "Zoom In"); icon: "zoom-in"; shortcut: Shortcut { sequence: StandardKey.ZoomIn }
            onTriggered: chartItem.zoom(true)
          }
        }
        ChartToolButton {
          taction: Taction {
            text: Noo.TR("QShortcut", "Zoom Out"); icon: "zoom-out"; shortcut: Shortcut { sequence: StandardKey.ZoomOut }
            onTriggered: chartItem.zoom(false)
          }
        }
        ChartToolButton {
          taction: Taction {
            text: qsTr("linear chart"); icon: "linearChart"
            onTriggered: chartItem.setChartType(true)
          }
          ButtonGroup.group: chartTypeGr
          checked: true
          checkable: true
        }
        ChartToolButton {
          taction: Taction {
            text: qsTr("bar chart"); icon: "barChart"
            onTriggered: chartItem.setChartType(false)
          }
          ButtonGroup.group: chartTypeGr
          checkable: true
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
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Y value:")
            font { pixelSize: upTextSize }
            color: activPal.text;  verticalAlignment: Text.AlignVCenter
            height: toolBar.height * 0.5
          }
          ComboBox {
            id: yCombo
            width: analyzeWindow.width / 7; height: toolBar.height * 0.48
            font { pixelSize: toolBar.height / 5 }
            model: chartItem.yValueActions
            delegate: ItemDelegate { text: modelData }
            currentIndex: chartItem.yValue
            onActivated: chartItem.yValue = index
            background: Rectangle {
              anchors.fill: parent
              color: activPal.base
            }
          }
        }
        Item { height: 2; width: analyzeWindow.width / 100 }
        Column {
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("ordered by:", "Keep a proper form - whole sentence will be: ordered by: question number, key signature, etc...")
            font { pixelSize: upTextSize }
            color: activPal.text; verticalAlignment: Text.AlignVCenter
            height: toolBar.height * 0.5
          }
          ComboBox {
            id: orderCombo
            width: analyzeWindow.width / 7; height: toolBar.height * 0.48
            font { pixelSize: toolBar.height / 5 }
            model: chartItem.xOrderActions
            delegate: ItemDelegate { text: modelData }
            currentIndex: chartItem.xOrder
            onActivated: chartItem.xOrder = index
            background: Rectangle {
              anchors.fill: parent
              color: activPal.base
            }
          }
        }
        Item { height: 2; width: analyzeWindow.width / 100 }
        Column {
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("student name:")
            font { pixelSize: upTextSize }
            color: activPal.text; verticalAlignment: Text.AlignVCenter
            height: toolBar.height * 0.5
          }
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: chartItem.userName === "" ? "-----" : chartItem.userName
            font { pixelSize: toolBar.height / 4; bold: true }
            color: activPal.text;  verticalAlignment: Text.AlignVCenter
            height: toolBar.height * 0.5
          }
        }
        Item { height: 2; width: analyzeWindow.width / 100 }
        Column {
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("level:")
            font { pixelSize: upTextSize }
            color: activPal.text; verticalAlignment: Text.AlignVCenter
            height: toolBar.height * 0.5
          }
          Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: levelText.width + Noo.fontSize(); height: toolBar.height / 2
            color: levelArea.containsMouse ? activPal.highlight : "transparent"
            radius: height / 5
            Text {
              id: levelText
              anchors.centerIn: parent
              text: chartItem.levelName === "" ? "-----" : chartItem.levelName
              font { pixelSize: toolBar.height / 4; bold: true }
              color: activPal.text
            }
            MouseArea {
              id: levelArea
              hoverEnabled: chartItem.levelName !== ""
              anchors.fill: parent
              onClicked: {
                var c = Qt.createComponent("qrc:/charts/LevelPopup.qml")
                var lp = c.createObject(analyzeWindow.contentItem)
                chartItem.fillPreview(lp.levelPreview)
              }
            }
          }
        }
        Item { height: 2; width: analyzeWindow.width / 100 }
        Column {
          Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: chartItem.questionCount
            font { pixelSize: upTextSize }
            color: activPal.text; horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            height: toolBar.height * 0.5
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
        onExamChanged: {
          if (helpTip && exam)
            helpTip.destroy()
        }
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

  Component.onCompleted: {
    if (allowOpen) {
      var h = Qt.createComponent("qrc:/charts/ChartHelpTip.qml")
      helpTip = h.createObject(analyzeWindow.contentItem, { "text": chartItem.chartHelpText() } )
    }
    SOUND.stop()
  }
}
