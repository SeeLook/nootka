/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019-2020 by Tomasz Bojczuk (seelook@gmail.com)     *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtQuick.Controls 2.2


Flickable {
  id: chartView

  property alias list: list
  property real sc: 1.0
  property real lThick: (height / 150.0) * sc
  property real maxDataHeight: chartView.height * 0.7333333333333333 *sc

  boundsBehavior: Flickable.StopAtBounds
  clip: true
  contentHeight: height * sc

  Timer { id: zoomTimer; interval: 100 }
  MouseArea {
    anchors.fill: parent
    enabled: list.count > 0
    onWheel: {
      if (wheel.modifiers & Qt.ControlModifier) {
        if (wheel.angleDelta.y > 0) {
          if (!zoomTimer.running) {
            sc = Math.min(2.0, sc * 1.125)
            zoomTimer.running = true
          }
        } else if (wheel.angleDelta.y < 0) {
          if (!zoomTimer.running) {
            sc = Math.max(0.5, sc * 0.888889)
            zoomTimer.running = true
          }
        }
      } else
          wheel.accepted = false
    }
  }

  Repeater { // Y grid lines
    model: chartItem.yAxisGridModel.length
    Rectangle {
      visible: list.count > 0; z: 0
      y: contentHeight * 0.1 + maxDataHeight - (chartItem.yAxisGridModel[index] / chartItem.maxYValue()) * maxDataHeight
      color: Noo.alpha(activPal.text, 100)
      width: Math.min(parent.width - list.height / 12, list.contentWidth); height: lThick / 3
      x: list.height / 12
    }
  }

  Component {
    id: lineComp
    LinearDelegate {
      chart: chartItem
      chartNr: modelIndex
      height: chartView.height * sc; width: height / 6
    }
  }

  Component {
    id: barComp
    BarDelegate {
      chart: chartItem
      groupNr: modelIndex
      height: chartView.height * sc; width: height / 6
    }
  }

  ListView {
    id: list
    z: 1
    width: parent.width; height: chartView.height * sc

    orientation: ListView.Horizontal
    boundsBehavior: Flickable.StopAtBounds

    model: chartItem.chartModel

    delegate: Component {
      Loader {
        property int modelIndex: index
        z: 2000 - modelIndex; // for proper overlapping keep 'z' stack in reverse order as delegates are created
        sourceComponent: chartItem.chartType === 0 ? lineComp : barComp
      }
    }

    header: Item { // fake, just to force space on the chart left
      visible: list.count > 0
      height: list.height; width: height / 12
    }

    footer: Item {
      visible: list.count > 0
      height: list.height; width: height / 6
      Rectangle {
        color: activPal.text
        y: parent.height - parent.width + lThick / 2
        width: parent.width / 1.3; height: lThick
        Repeater { // X arrow
          model: 2
          Rectangle {
            x: parent.width
            color: activPal.text; radius: width / 2
            height: lThick; width: lThick * 5
            rotation: index === 0 ? 155 : -155; transformOrigin: Item.Left
          }
        }
      }
    }

    Rectangle { // Y axis (over that fake header)
      visible: list.count > 0
      color: Noo.alpha(activPal.base, 200)
      height: list.height; width: height / 12
      Rectangle { // Y axis line
        color: activPal.text
        x: parent.width; y: parent.height * 0.01 + lThick / 2
        width: lThick; height: parent.height * 0.8233333333333333
        Repeater { // Y arrow
          model: 2
          Rectangle {
            color: activPal.text; radius: width / 2
            width: lThick; height: lThick * 5
            rotation: index === 0 ? 25 : -25; transformOrigin: Item.Top
          }
        }
        Text { // Y label
          font { pixelSize: lThick * 5; bold: true }
          y: (parent.height - height) / 2; x: -width / 2 - height
          color: activPal.text
          text: chartItem.yAxisLabel
          rotation: -90
        }
      }
      Repeater { // ticks and Y values (text)
        model: chartItem.yAxisGridModel.length
        Rectangle {
          y: contentHeight * 0.1 + maxDataHeight - (chartItem.yAxisGridModel[index] / chartItem.maxYValue()) * maxDataHeight - lThick / 4
          color: activPal.text
          width: lThick * 1.5; height: lThick
          x: parent.width - width
          Text {
            color: activPal.text; text: index < chartItem.yAxisGridModel.length ? chartItem.yAxisTickText(index) : ""
            font { pixelSize: lThick * 3.5 }
            y: -height + lThick / 2; x: lThick * 3
          }
        }
      }
    }
    ScrollBar.horizontal: ScrollBar { active: true; visible: true }
  }

  ScrollBar.vertical: ScrollBar { active: true; visible: true }
}
