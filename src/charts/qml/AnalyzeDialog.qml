/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2018-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
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
  property var chDrawer: null

  function openExam() {
    if (allowOpen)
      chartItem.openExam()
  }

  function openDrawer() { drawerAct.trigger() }

  visible: true
  modality: Qt.WindowModal
  title: chartItem.chartWindowTitle
  width: nootkaWindow.width; height: nootkaWindow.height; x: nootkaWindow.x; y: nootkaWindow.y
  color: activPal.base

  ButtonGroup { id: chartTypeGr }

  Column {
    ToolBar {
      id: toolBar
      z: 255
      height: maxButt.height; width: analyzeWindow.width
      background: TipRect { anchors.fill: parent; color: activPal.window; radius: 0 }
      Row {
        ChartToolButton {
          taction: Taction {
            id: drawerAct
            text: qsTr("Settings of a chart"); icon: "chartSett"
            onTriggered: {
              if (chDrawer) {
                  if (chDrawer.visible)
                    chDrawer.close()
                  else
                    chDrawer.open()
              } else {
                  var d = Qt.createComponent("qrc:/charts/ChartDrawer.qml")
                  chDrawer = d.createObject(analyzeWindow)
              }
            }
          }
        }
        ChartToolButton {
          taction: Taction {
            text: Noo.TR("QShortcut", "Zoom In"); icon: "zoom-in"; shortcut: Shortcut { sequence: StandardKey.ZoomIn }
            onTriggered: chartView.sc = Math.min(2.0, chartView.sc * 1.125)
          }
        }
        ChartToolButton {
          taction: Taction {
            text: Noo.TR("QShortcut", "Zoom Out"); icon: "zoom-out"; shortcut: Shortcut { sequence: StandardKey.ZoomOut }
            onTriggered: chartView.sc = Math.max(0.5, chartView.sc * 0.888889)
          }
        }
        ChartToolButton {
          id: maxButt
          taction: Taction {
            text: Noo.TR("QMdiSubWindow", "Maximize"); icon: "fullscreen"
            onTriggered: visibility = visibility === 2 ? 4 : 2
          }
        }
        Item { height: 2; width: (analyzeWindow.width - (maxButt.x + maxButt.width) - exitButt.width - infoRow.width) / 2 }

        Row {
          id: infoRow
//             visible: chartItem.questionCount > 0
          Column {
            visible: chartItem.isMelody // && chartItem.questionCount > 0
            Text {
              anchors.horizontalCenter: parent.horizontalCenter
              text: qsTr("Y value:")
              font { pixelSize: upTextSize }
              color: activPal.text;  verticalAlignment: Text.AlignVCenter
              height: toolBar.height * 0.48
            }
            Text {
              anchors.horizontalCenter: parent.horizontalCenter
              text: chartItem.yValueActions[chartItem.yValue]
              font { pixelSize: upTextSize; bold: true }
              color: activPal.text; verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
              height: toolBar.height * 0.48; width: analyzeWindow.width / 7
              fontSizeMode: Text.Fit; minimumPixelSize: Noo.fontSize() / 2
            }
          }
          Item { height: 2; width: analyzeWindow.width / 100 }
          Column {
            visible: !chartItem.isMelody //&& chartItem.questionCount > 0
            Text {
              anchors.horizontalCenter: parent.horizontalCenter
              text: qsTr("ordered by:", "Keep a proper form - whole sentence will be: ordered by: question number, key signature, etc...")
              font { pixelSize: upTextSize }
              color: activPal.text; verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
              height: toolBar.height * 0.48; width: analyzeWindow.width / 7
            }
            Text {
              anchors.horizontalCenter: parent.horizontalCenter
              text: chartItem.xOrderActions[chartItem.xOrder]
              font { pixelSize: upTextSize; bold: true }
              color: activPal.text; verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
              height: toolBar.height * 0.48; width: analyzeWindow.width / 7
              fontSizeMode: Text.Fit; minimumPixelSize: Noo.fontSize() / 2
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
        } // Row (infoRow)
      } // Row


      ChartToolButton {
        id: exitButt
        anchors.right: parent.right
        taction: Taction {
          text: Noo.TR("QPlatformTheme", "Close"); icon: "exit"
          onTriggered: close()
        }
      }
    }

    property TchartItem chartItem: TchartItem {
      id: chartItem
      tipItem: tip.tipItem
      parent: chartView; anchors.fill: parent
      ChartTip { id: tip; parent: chartView.list }
      onExamChanged: {
        if (helpTip && exam)
          helpTip.destroy()
      }
      onResetChartPos: chartView.list.positionViewAtBeginning()
      onLoadExamFailed: {
        if (!helpTip) {
            var e = Qt.createComponent("qrc:/charts/ChartHelpTip.qml")
            helpTip = e.createObject(analyzeWindow.contentItem)
        }
        helpTip.text = message
        helpTip.hintColor = GLOB.wrongColor
      }
    }

    ChartView {
      id: chartView
      width: analyzeWindow.width - (chDrawer && chDrawer.pinned && chDrawer.visible ? chDrawer.width : 0)
      height: analyzeWindow.height - toolBar.height
      transform: Translate {
        x: chDrawer ? chDrawer.position * analyzeWindow.height * 0.4 : 0
      }
    }
  }

  onClosing: {
    SOUND.startListen()
    analyzeWindow.destroy()
  }

  Component.onCompleted: {
    if (allowOpen) {
      var h = Qt.createComponent("qrc:/charts/ChartHelpTip.qml")
      helpTip = h.createObject(analyzeWindow.contentItem, { "text": chartItem.chartHelpText() } )
    }
    SOUND.stop()
    if (chartItem.keepDrawerOpened())
      openDrawer()
  }
}
