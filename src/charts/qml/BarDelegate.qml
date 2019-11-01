/** This file is part of Nootka (http://nootka.sf.net)               *
 * Copyright (C) 2019 by Tomasz Bojczuk (seelook@gmail.com)          *
 * on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)   */

import QtQuick 2.9
import QtGraphicalEffects 1.0

import Nootka.Charts 1.0


Item {
  id: barDel

  property alias chart: barIt.chart
  property alias groupNr: barIt.groupNr

  TbarChartDelegate {
    id: barIt
    x: parent.width * 0.75
    y: parent.height * 0.1 + maxDataHeight - (yValue / chartItem.maxYValue()) * maxDataHeight
    width: parent.width / 2; height: maxDataHeight * 1.14 - y
    visible: false
    hovered: barMA.containsMouse
  }
  DropShadow {
    z: 5010
    anchors.fill: barIt
    horizontalOffset: barDel.width / 30
    verticalOffset: horizontalOffset
    radius: Noo.fontSize() * (barIt.hovered ? 2 : 1)
    samples: 1 + radius * 2
    color: activPal.shadow
    source: barIt
    Behavior on radius { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    scale: barMA.containsMouse ? 1.1 : 1.0
    Behavior on scale { enabled: GLOB.useAnimations; NumberAnimation { duration: 150 }}
    MouseArea {
      id: barMA
      anchors.fill: parent
      hoverEnabled: true
    }
  }

  Rectangle { // X axis line (partial)
    color: activPal.text
    y: parent.height - width + lThick / 2
    width: parent.width; height: lThick
    Rectangle { // X tick
      color: activPal.text
      x: parent.width - lThick; y: lThick
      width: lThick; height: lThick * 2
      Column { // X value (description)
        y: lThick * 2
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
          text: barIt.xText; font { pixelSize: barDel.width / 5 }
          anchors.horizontalCenter: parent.horizontalCenter
          color: activPal.text; textFormat: Text.RichText; horizontalAlignment: Text.AlignHCenter
        }
      }
    }
  }

}
