/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019 by Tomasz Bojczuk (seelook@gmail.com)          *
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

  Repeater {
    model: chartItem.yAxisGridModel.length
    Rectangle {
      visible: list.count > 0; z: 0
      y: maxDataHeight * 0.1 + maxDataHeight - (chartItem.yAxisGridModel[index] / chartItem.maxYValue()) * maxDataHeight
      color: Noo.alpha(activPal.text, 100)
      width: parent.width - list.height / 12; height: lThick / 2
      x: list.height / 12
    }
  }
  
  ListView {
    id: list
    z: 1
    width: parent.width; height: chartView.height * sc
    
    orientation: ListView.Horizontal
    boundsBehavior: Flickable.StopAtBounds

    model: chartItem.chartModel

    delegate: LinearDelegate {
      chart: chartItem
      chartNr: index
      height: chartView.height * sc; width: height / 6
    }

    header: Item { // fake, just to force space on the left
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
      }
    }

    Rectangle { // Y axis (over that fake header)
      visible: list.count > 0
      color: Noo.alpha(activPal.base, 200)
      height: list.height; width: height / 12
      Rectangle {
        color: activPal.text
        x: parent.width; y: parent.height / 20 + lThick / 2
        width: lThick; height: parent.height * 0.7833333333333333
        Repeater {
          model: 2
          Rectangle {
            color: activPal.text; radius: width / 2
            width: lThick; height: lThick * 5
            rotation: index === 0 ? 25 : -25; transformOrigin: Item.Top
          }
        }
        Text {
          font { pixelSize: lThick * 6; bold: true }
          y: (parent.height - height) / 2; x: -list.height / 8
          color: activPal.text
          text: chartItem.yAxisLabel
          rotation: -90
        }
      }
      Repeater {
        model: chartItem.yAxisGridModel.length
        Rectangle {
          y: maxDataHeight * 0.1 + maxDataHeight - (chartItem.yAxisGridModel[index] / chartItem.maxYValue()) * maxDataHeight - lThick / 4
          color: activPal.text
          width: lThick * 1.5; height: lThick
          x: parent.width - width
          Text {
            color: activPal.text; text: chartItem.timeFormated(chartItem.yAxisGridModel[index])
            font { pixelSize: lThick * 4 }
            y: -height; x: lThick * 3
          }
        }
      }
    }
    ScrollBar.horizontal: ScrollBar { active: true; visible: true }
  }

  ScrollBar.vertical: ScrollBar { active: true; visible: true }
}
